#include "NetworkManager.h"
#include "Packet.h"
#include "Scene.h"
#include "GameObject.h"
#include <thread>
#include <mutex>
#include <windows.h>
#include <string>
#include <queue>
#include <functional>
#include "Gameframework.h"


void LogDebugOutput(const std::string& message) {
    OutputDebugStringA((message + "\n").c_str());
}


std::mutex packetMutex;
std::mutex eMutex;
#define BUFSIZE 1024

//���� �̿��� �κ��� �ּ� ó����.

NetworkManager::NetworkManager()
    : m_socket(INVALID_SOCKET), m_initialized(false)
{
    
}

NetworkManager::~NetworkManager()
{
    
    Disconnect();
    if (m_initialized)
    {
        WSACleanup();
    }
}

bool NetworkManager::Initialize()
{
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::cerr << "WSAStartup failed: " << result << std::endl;
        return false;
    }
    m_initialized = true;
    return true;
}

bool NetworkManager::ConnectToServer(const std::string& serverIP, int serverPort)
{
    if (!m_initialized)
    {
        if (!Initialize())
        {
            return false;
        }
    }

    m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_socket == INVALID_SOCKET)
    {
        std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
        WSACleanup();
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    int result = connect(m_socket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Unable to connect to server: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }




    // ���� ���� ���� (��� ����)
    m_serverIP = serverIP;
    m_serverPort = serverPort;

    // �����κ��� ������ ������ ó���ϴ� ������ ����    ó�� �õ��� ���
    /*std::thread receiveThread(&NetworkManager::ReceiveThread, this, m_pPlayer, scene);
    receiveThread.detach();*/

    return true;
}


void NetworkManager::Disconnect()
{
    if (m_socket != INVALID_SOCKET)
    {
        closesocket(m_socket);
        m_socket = INVALID_SOCKET;
    }
}

//--------------------------------------------------------------------------------------------------------���⼭���� ���� (11/09)
bool NetworkManager::SendData(void* packet) {

    PacketType packetType = *(PacketType*)packet;
    int result;

    // ��Ŷ Ÿ�� ����
    /*result = send(m_socket, (char*)&packetType, sizeof(PacketType), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "[Error] Send failed (packet type): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "[Debug] Sent packet type: " << packetType << std::endl;*/

    // ��Ŷ ������ ����
    switch (packetType) {
    case KEY_CONTROL_PACKET: {
        PlayerKeyControl* keyControlPacket = static_cast<PlayerKeyControl*>(packet);
        result = send(m_socket, (char*)keyControlPacket, sizeof(PlayerKeyControl), 0);
        if (result == SOCKET_ERROR) {
            std::string udpError = "[Error] TCP send failed: " + std::to_string(WSAGetLastError());
            LogDebugOutput(udpError);
        }
        else {
            std::cout << "[Debug] Sending Key Packet: Key = " << keyControlPacket->nMessageID << std::endl;
            std::string debugMessage = "[Debug] Sending Key Packet: Key = " + std::to_string(keyControlPacket->nMessageID);
            LogDebugOutput(debugMessage);
        }
        
        break;
    }
    case MESH_PACKET: {
        PlayerMeshPacket* meshPacket = static_cast<PlayerMeshPacket*>(packet);

        /*std::string debugMessage = "Packet Data - Type: MESH_PACKET, fx: " +
            std::to_string(meshPacket->m_fxPosition) +
            ", fy: " + std::to_string(meshPacket->m_fyPosition) +
            ", meshIndex: " + std::to_string(meshPacket->meshIndex) +
            ", Rotation: " + std::to_string(meshPacket->m_fRotation);
        LogDebugOutput(debugMessage);*/
        result = send(m_socket, (char*)meshPacket, sizeof(PlayerMeshPacket), 0);
        if (result == SOCKET_ERROR) {
            std::string udpError = "[Error] TCP send failed: " + std::to_string(WSAGetLastError());
            LogDebugOutput(udpError);
        }
        else {
            /*std::string udpSuccess = "[Debug] Sent Mesh Packet via TCP (Index: " + std::to_string(meshPacket->meshIndex) + ")";
            LogDebugOutput(udpSuccess);*/
        }
        break;
    }
    default:
        std::cerr << "[Error] Unknown packet type." << std::endl;
        return false;
    }

    if (result == SOCKET_ERROR) {
        std::cerr << "[Error] Send failed (packet data): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }

    std::cout << "[Debug] Packet data sent. Bytes: " << result << std::endl;
    return true;
}

void NetworkManager::Client_Send_Thread(Player* player, Scene* scene) {
    auto hasMeshPacket = false;
    auto hasKeyControlPacket = false;

    PlayerMeshPacket meshPacket{};
    meshPacket.packetType = MESH_PACKET;
    meshPacket.size = sizeof(PlayerMeshPacket);

    PlayerKeyControl keyControlPacket{};
    keyControlPacket.packetType = KEY_CONTROL_PACKET;
    keyControlPacket.size = sizeof(PlayerKeyControl);

    // ���� ��ǥ�� �����ϴ� ����
    float lastFxPosition = 0.0f;
    float lastFyPosition = 0.0f;
    //float lastFRotaion = 0.0f;

    meshPacket.meshIndex = scene->m_nIndex; 
    this->SendData(&meshPacket);

    while (scene->m_bGameStop == false) {  // scene�� m_bGameStop�� ����Ͽ� ���� ���� ���� ����
        hasMeshPacket = false;
        hasKeyControlPacket = false;

        packetMutex.lock();

        if (scene->m_pNextGameObjectOne) {
            if (!scene->m_objects.empty() && scene->m_nIndexPlayerOne >= 0 &&
                scene->m_nIndexPlayerOne < scene->m_objects.size()) {

                GameObject* activeBlock = scene->m_objects[scene->m_nIndexPlayerOne];  //���� �������� ���� ����.
                // ��Ŷ ������ ����

                if (activeBlock->m_fxPosition != lastFxPosition || activeBlock->m_fyPosition != lastFyPosition) { //���� ��ġ�� �ٲ�� ����
                    lastFxPosition = activeBlock->m_fxPosition;
                    lastFyPosition = activeBlock->m_fyPosition;
                    //lastFRotaion = activeBlock->m_fRotation;

                    meshPacket.m_fxPosition = activeBlock->m_fxPosition;
                    meshPacket.m_fyPosition = activeBlock->m_fyPosition;
                    //meshPacket.m_fRotation = activeBlock->m_fRotation;
                    meshPacket.meshIndex = scene->m_nIndex;
                    meshPacket.topPositionRate = activeBlock->m_pBody->GetPosition().y;

                    hasMeshPacket = true;
                    /*std::string debugMessage = "Sending Packet: fx: " + std::to_string(activeBlock->m_fxPosition) +
                        ", fy: " + std::to_string(activeBlock->m_fyPosition) +
                        ", meshIndex: " + std::to_string(scene->m_nIndex) +
                        "ž ������: " + std::to_string(activeBlock->m_pBody->GetPosition().y);
                    LogDebugOutput(debugMessage);*/
                }
            }
        }

        // Ű �Է��� ���� ��� Ű �Է� ���� ��Ŷ ����
        if (scene->m_bKeyInput) {
            switch (scene->m_lastKeyPressed) {
            case VK_LEFT:
                keyControlPacket.nMessageID = VK_LEFT;
                break;
            case VK_RIGHT:
                keyControlPacket.nMessageID = VK_RIGHT;
                break;
            case VK_UP:
                keyControlPacket.nMessageID = VK_UP;
                break;
            case VK_DOWN:
                keyControlPacket.nMessageID = VK_DOWN;
                break;
            case VK_OEM_COMMA:
                keyControlPacket.nMessageID = VK_OEM_COMMA;
                break;
            case VK_OEM_PERIOD:
                keyControlPacket.nMessageID = VK_OEM_PERIOD;
                break;
            default:
                keyControlPacket.nMessageID = 0;
                break;
            }

            // ����� �޽���
            //std::cout << "[Debug] Sending Key Packet: Key = " << keyControlPacket.nMessageID << std::endl;
            //std::string debugMessage = "[Debug] Sending Key Packet: Key = " + std::to_string(keyControlPacket.nMessageID);
            //LogDebugOutput(debugMessage);

            if (keyControlPacket.nMessageID != 0)
            {
                hasKeyControlPacket = true;
            }
        }

        packetMutex.unlock();

        if (hasMeshPacket)
        {
            this->SendData(&meshPacket);
            if (!this->SendData(&meshPacket))
            {
                std::cerr << "[Error] Failed to send mesh packet to server." << std::endl;
            }
        }

        if (hasKeyControlPacket)
        {
            //this->SendData(static_cast<void*>(&keyControlPacket));
        }

        Sleep(200); // ���� �ֱ� (0.1��)
    }
}




void NetworkManager::ReceiveThread(Player* pPlayer, Scene* scene) {
    char buffer[BUFSIZE];
    int result;
  
    // ���� ��ǥ�� �����ϴ� ����
    float lastFxPosition = 0.0f;
    float lastFyPosition = 0.0f;

    // ���� �޽� �ε����� �����ϱ� ���� ������ Scene Ŭ������ �߰�
    int previousMeshIndex = -1; // �ʱⰪ�� -1�� ������ �������� �� ����
    


    while (true) {
        
        

        result = recv(m_socket, buffer, BUFSIZE, 0);
        
    
        // ���� ��ǥ�� �����Ͽ� ���� ���θ� Ȯ��
        float lastFxPosition = 0.0f;
        float lastFyPosition = 0.0f;
        
        

        if (result > 0) {
            // ���� �����͸� PacketType���� �Ľ�
            PacketType* packetType = reinterpret_cast<PacketType*>(buffer);
            switch (*packetType) {
            
            case MESH_PACKET: {
                // MESH_PACKET ó��
                PlayerMeshPacket* meshPacket = reinterpret_cast<PlayerMeshPacket*>(buffer);

                if (scene) {
                  //----------------------------------------------------------------------------------------���� ���� ���� �˷��ִ� ��� ���� �޽� Ÿ���� �������� ���� �ɷ� ����
                    if (meshPacket->meshIndex >= 0 && meshPacket->meshIndex < scene->m_nRandomCount) {

                        if (scene->m_pNextGameObjectTwo) {
                            scene->m_pNextGameObjectTwo->SetMesh(scene->m_pRandomMeshGet[meshPacket->meshIndex].SelectMesh);
                        }
                        else if (!scene->m_pNextGameObjectTwo) {
                            scene->m_pNextGameObjectTwo = new GameObject();
                            scene->m_pNextGameObjectTwo->SetMesh(scene->m_pRandomMeshGet[meshPacket->meshIndex].SelectMesh);
                        }

                    //    //---------------------------------------------------------------------12/06 ������ 
                        // scene->m_objects2.size() > 0 && scene->m_nIndexPlayerTwo >= 0 && scene->m_nIndexPlayerTwo < scene->m_objects2.size() �� �����Ǹ� ��Ÿ�Ӻ��������Ʈ�� 
                        // �� ���� �����, b2Vec2 velocity �����ؼ� �����⸸ �Ǵ� ��, scene->m_objects2[scene->m_nIndexPlayerTwo]�� x,y ���� �������� ���� �ɷ� �����ص� b2Vec2 velocity�θ� ������;;;;
                        // b2Vec2 velocity�� 0.0f, 0.0f�� �����ϸ� �����ϰ� ���������� ����....

                        // �޽� �ε����� ����Ǿ��ٸ�
                        if (meshPacket->meshIndex != scene->m_nIndex2) {
                            
                            // ���ο� �޽��� ����
                            scene->m_nIndex2 = meshPacket->meshIndex;
                            lastFxPosition = meshPacket->m_fxPosition;
                            lastFyPosition = meshPacket->m_fyPosition;


                                if (scene->m_objects2.size() > 0 && scene->m_nIndexPlayerTwo >= 0 &&
                                    scene->m_nIndexPlayerTwo < scene->m_objects2.size()) {
                                    
                                    if (scene->m_bTriggerActive2) {

                                        scene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, scene->m_pPlayer2);	//�ش� �ε����� ���� ����
                                        b2Vec2 velocity(lastFxPosition*-1, lastFyPosition*-1);
                                        scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                                        GameObject* activeBlock = scene->m_objects2[scene->m_nIndexPlayerTwo];
                                        if (activeBlock) {
                                            activeBlock->m_fxPosition = meshPacket->m_fxPosition;
                                            activeBlock->m_fyPosition = meshPacket->m_fyPosition;
                                            
                                            
                                        }
                                        scene->m_pNextGameObjectTwo = scene->RandomMesh(scene->m_pPlayer2);


                                    }
                                    else if (!scene->m_bTriggerActive2)
                                    {
                                        b2Vec2 velocity(lastFxPosition*-1, lastFyPosition*-1);
                                        scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                                        if (scene->m_objects2[scene->m_nIndexPlayerTwo]) {
                                            scene->m_objects2[scene->m_nIndexPlayerTwo]->m_fxPosition = meshPacket->m_fxPosition;
                                            scene->m_objects2[scene->m_nIndexPlayerTwo]->m_fyPosition = meshPacket->m_fyPosition;
                                            
                                        }
                                    }
                                }
                               

                            


                        }




                    }    

                }


               
              
                break;
            }
            case KEY_CONTROL_PACKET: {
                // KEY_CONTROL_PACKET ó��
                PlayerKeyControl* keyPacket = reinterpret_cast<PlayerKeyControl*>(buffer);

                

                // �ٿ�Ű�� ���� ����
                //if (keyPacket->nMessageID == VK_DOWN) {
                    //if (scene->m_bTriggerActive2){
                    //         
                    //    		scene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, scene->m_pPlayer2);	//�ش� �ε����� ���� ����
                    //            scene->m_bTriggerActive2 = false;	//������ ������ �� �ְ� ������ְ�
                    //    		b2Vec2 velocity(0.0f, -50.0f);
                    //            scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                    //            scene->m_pNextGameObjectTwo = scene->RandomMesh(scene->m_pPlayer2);
                    //            
                    //
                    //}
                    //else if (!scene->m_bTriggerActive2)
                    //{
                    //    		b2Vec2 velocity(0.0f, -500.0f);
                    //            scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                    //}

                //}

                std::string debugMessage = "Ű�� ����: "
                    + std::to_string(keyPacket->nMessageID);
                LogDebugOutput(debugMessage);
                
                break;
            }
                                   //epacketMutex.unlock();
            default: {
                /*std::string debugMessage = "[Error] Unknown packet type received.";
                LogDebugOutput(debugMessage);*/
                break;
            }
            }
            //int timeData;
            //if (ReceiveTimeData(timeData))
            //{
            //    //���� �ð��� ���ӿ� �ݿ� (���� �ʿ�)
            //}
            //else
            //{
            //    break; // ���� ���� �Ǵ� ���� �߻� �� ���� ����
            //}
        }
        else if (result == 0) {
            std::string debugMessage = "Connection closed by server.";
            LogDebugOutput(debugMessage);
            break; // ������ ������ ������ ���
        }
        else {
            std::string debugMessage = "Receive failed: " + std::to_string(WSAGetLastError());
            LogDebugOutput(debugMessage);
            break; // ���� �߻� �� ����
        }
    

        Sleep(200); // CPU ��뷮�� ���߱� ���� �ణ�� ���
    }
    
    

    std::string debugMessage = "Receive thread terminated.";
    LogDebugOutput(debugMessage);
}

bool NetworkManager::ReceiveTimeData(int& timeData)
{
    if (m_socket == INVALID_SOCKET)
    {
        std::cerr << "Socket is not connected." << std::endl;
        return false;
    }

    char buffer[sizeof(int)];
    int result = recv(m_socket, buffer, sizeof(int), 0);
    if (result > 0)
    {
        int timeData = *reinterpret_cast<int*>(buffer); // ��Ʈ��ũ ����Ʈ ���� -> ȣ��Ʈ ����Ʈ ����
       /* std::string debugMessage = "[Debug] Received Time from Server: " + std::to_string(timeData) + " seconds";
        LogDebugOutput(debugMessage);*/
        return true;
    }
    else if (result == 0)
    {
        std::cerr << "Connection closed by server." << std::endl;
    }
    else
    {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    }
    return false;
}

bool NetworkManager::ReceiveStartSignal(bool& startSignal) {
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Socket is not connected." << std::endl;
        return false;
    }

    char buffer[sizeof(bool)];
    int result = recv(m_socket, buffer, sizeof(bool), 0);
    if (result > 0) {
        startSignal = *reinterpret_cast<bool*>(buffer);
        return true;
    }
    else if (result == 0) {
        std::cerr << "Connection closed by server." << std::endl;
    }
    else {
        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    }
    return false;
}

//------------------------------------------������ ������ ���� �������� �޴� �Լ� 12/07 
bool NetworkManager::ReceivePlayerCount(int& playerCount) {
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[Error] Socket is not connected." << std::endl;
        return false;
    }

    // �����κ��� count ���� ����
    char buffer[sizeof(int)] = { 0 };
    int result = recv(m_socket, buffer, sizeof(int), 0);

    if (result > 0) {
        playerCount = ntohl(*reinterpret_cast<int*>(buffer));
        std::string debugMessage = "Received Player Count from Server: " + std::to_string(playerCount);
        LogDebugOutput(debugMessage);
        return true;
    }
    else if (result == 0) {
        std::cerr << "[Error] Connection closed by server." << std::endl;
    }
    else {
        std::cerr << "[Error] Receive failed: " << WSAGetLastError() << std::endl;
    }
    return false;
}
