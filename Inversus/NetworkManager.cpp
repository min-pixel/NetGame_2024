#include "NetworkManager.h"
#include "Packet.h"
#include "Scene.h"
#include "GameObject.h"
#include <thread>
#include <mutex>
#include <windows.h>
#include <string>

void LogDebugOutput(const std::string& message) {
    OutputDebugStringA((message + "\n").c_str());
}


std::mutex packetMutex;
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


//bool NetworkManager::SendAcceptServerPacket() {
//    if (m_socket == INVALID_SOCKET) {
//        std::cerr << "Socket is not connected." << std::endl;
//        return false;
//    }
//
//    bool accept_server = true;
//
//    int result = send(m_socket, (char*)&accept_server, sizeof(accept_server), 0);
//    if (result == SOCKET_ERROR) {
//        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
//        closesocket(m_socket);
//        return false;
//    }
//
//    std::cout << "Sent accept_server signal (true) to server." << std::endl;
//    return true;
//}



//bool NetworkManager::SendMessageFunc(const std::string& data)
//{
//    if (m_socket == INVALID_SOCKET)
//    {
//        std::cerr << "Socket is not connected." << std::endl;
//        return false;
//    }
//
//    int result = send(m_socket, data.c_str(), static_cast<int>(data.length()), 0);
//    if (result == SOCKET_ERROR)
//    {
//        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
//        closesocket(m_socket);
//        return false;
//    }
//
//    std::cout << "Bytes Sent: " << result << std::endl;
//    return true;
//}
////--------------------------------------------------------------�޼����� ������ �� �Ӹ� �ƴ϶�, �����͸� ������ send�Լ��� �ʿ� 
//// ����ó : void Scene::BuildObjects()���� m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);�� ������ ��
//
////--------------------------------------------------------------�������� ������ �����͸� ���� ���ú� �Լ��� �ʿ���
//bool NetworkManager::ReceiveData(std::string& outData)
//{
//    if (m_socket == INVALID_SOCKET)
//    {
//        std::cerr << "Socket is not connected." << std::endl;
//        return false;
//    }
//
//    char recvbuf[DEFAULT_BUFFER_LENGTH];
//    int result = recv(m_socket, recvbuf, DEFAULT_BUFFER_LENGTH, 0);
//
//    if (result > 0)
//    {
//        outData.assign(recvbuf, result);
//        return true;
//    }
//    else if (result == 0)
//    {
//        std::cerr << "Connection closed by server." << std::endl;
//    }
//    else
//    {
//        std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
//    }
//
//    return false;
//}


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
            std::string udpSuccess = "[Debug] Sent Mesh Packet via TCP (Index: ";
            LogDebugOutput(udpSuccess);
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
    
    PlayerMeshPacket meshPacket = {};  // ��� ���� 0���� �ʱ�ȭ 
    PlayerKeyControl keyControlPacket = {}; // ��� ���� 0���� �ʱ�ȭ 

    
    

    // ���� ��ǥ�� �����ϴ� ����
    float lastFxPosition = 0.0f;
    float lastFyPosition = 0.0f;
    //float lastFRotaion = 0.0f;
    

    while (scene->m_bGameStop == false) {  // scene�� m_bGameStop�� ����Ͽ� ���� ���� ���� ����
        std::lock_guard<std::mutex> lock(packetMutex);
        if (scene->m_pNextGameObjectOne) {

            if (!scene->m_objects.empty() && scene->m_nIndexPlayerOne >= 0 &&
                scene->m_nIndexPlayerOne < scene->m_objects.size()) {

                GameObject* activeBlock = scene->m_objects[scene->m_nIndexPlayerOne];  //���� �������� ���� ����.
                // ��Ŷ ������ ����

                //if (activeBlock->m_fxPosition != lastFxPosition || activeBlock->m_fyPosition != lastFyPosition) { //���� ��ġ�� �ٲ�� ����
                    lastFxPosition = activeBlock->m_fxPosition;
                    lastFyPosition = activeBlock->m_fyPosition;
                    //lastFRotaion = activeBlock->m_fRotation;

                    meshPacket.packetType = MESH_PACKET;
                    meshPacket.m_fxPosition = activeBlock->m_fxPosition;
                    meshPacket.m_fyPosition = activeBlock->m_fyPosition;
                    //meshPacket.m_fRotation = activeBlock->m_fRotation;
                    meshPacket.meshIndex = scene->m_nIndex;
                    meshPacket.topPositionRate = activeBlock->m_pBody->GetPosition().y;
                    
                    // ������ ��Ŷ ����
                    this->SendData(&meshPacket);
                    if (!this->SendData(&meshPacket)) {
                        std::cerr << "[Error] Failed to send mesh packet to server." << std::endl;
                    }


                    /*std::string debugMessage = "Sending Packet: fx: " + std::to_string(activeBlock->m_fxPosition) +
                        ", fy: " + std::to_string(activeBlock->m_fyPosition) +
                        ", meshIndex: " + std::to_string(scene->m_nIndex) +
                        "ž ������: " + std::to_string(activeBlock->m_pBody->GetPosition().y);
                    LogDebugOutput(debugMessage);*/



                //}

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
                break;
            }

            // ����� �޽���
            //std::cout << "[Debug] Sending Key Packet: Key = " << keyControlPacket.nMessageID << std::endl;
            //std::string debugMessage = "[Debug] Sending Key Packet: Key = " + std::to_string(keyControlPacket.nMessageID);
            //LogDebugOutput(debugMessage);

            keyControlPacket.packetType = KEY_CONTROL_PACKET;
            
            this->SendData(static_cast<void*>(&keyControlPacket));
            



        }
        Sleep(100); // ���� �ֱ� (0.1��)
    
    }
}


void NetworkManager::ReceiveThread(Player* pPlayer2, Scene* scene) {
    char buffer[BUFSIZE];
    int result;

    while (true) {
        result = recv(m_socket, buffer, BUFSIZE, 0);

        if (result > 0) {
            // ���� �����͸� PacketType���� �Ľ�
            PacketType* packetType = reinterpret_cast<PacketType*>(buffer);

            switch (*packetType) {
            case MESH_PACKET: {
                // MESH_PACKET ó��
                PlayerMeshPacket* meshPacket = reinterpret_cast<PlayerMeshPacket*>(buffer);



                // Lock�� ����Ͽ� �����ϰ� �÷��̾� 2 ������Ʈ
                std::lock_guard<std::mutex> lock(packetMutex);

                if (pPlayer2 && scene) {

                  
                        // ���� �� Ÿ�� ������Ʈ
                    if (meshPacket->meshIndex >= 0 && meshPacket->meshIndex < scene->m_nRandomCount) {
                        Mesh* newMesh = scene->m_pRandomMeshGet[meshPacket->meshIndex].SelectMesh;
                        if (scene->m_pNextGameObjectTwo) {
                            scene->m_pNextGameObjectTwo->SetMesh(newMesh);
                        }
                        else {
                            scene->m_pNextGameObjectTwo = new GameObject();
                            scene->m_pNextGameObjectTwo->SetMesh(newMesh);
                        }
                    }
                       

                        //�� ����߸��� �� �õ�
                        //if (scene->m_bTriggerActive2)
                        //{
                        //    scene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, scene->m_pPlayer2);	//�ش� �ε����� ���� ����
                        //    scene->m_bTriggerActive2 = false;	//������ ������ �� �ְ� ������ְ�
                        //    b2Vec2 velocity(0.0f, -50.0f);
                        //    scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                        //    if (meshPacket->meshIndex >= 0 && meshPacket->meshIndex < scene->m_nRandomCount) {
                        //        Mesh* newMesh = scene->m_pRandomMeshGet[meshPacket->meshIndex].SelectMesh;
                        //        if (scene->m_pNextGameObjectTwo) {
                        //            scene->m_pNextGameObjectTwo->SetMesh(newMesh);
                        //        }
                        //        else {
                        //            scene->m_pNextGameObjectTwo = new GameObject();
                        //            scene->m_pNextGameObjectTwo->SetMesh(newMesh);
                        //        }
                        //    }
                        //    
                        //}
                        //else if (!scene->m_bTriggerActive2)
                        //{
                        //    b2Vec2 velocity(0.0f, -500.0f);
                        //    scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                        //}


                    

                }



                
                //if (scene->m_pNextGameObjectTwo!=nullptr) {
                //    std::lock_guard<std::mutex> lock(packetMutex);
                //    //scene->m_nIndex2 = meshPacket->meshIndex;
                //    if (!scene->m_objects2.empty() && scene->m_nIndexPlayerTwo >= 0 &&
                //        scene->m_nIndexPlayerTwo < scene->m_objects2.size()) {

                //        //cene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, scene->m_pPlayer2);	//�ش� �ε����� ���� ����
                //        
                //        //b2Vec2 velocity(0.0f, -50.0f);
                //        //scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);
                //        //scene->m_nIndex2 = meshPacket->meshIndex;

                //        GameObject* activeBlock2 = scene->m_objects2[scene->m_nIndexPlayerTwo];  //���� �������� ���� ����.
                //        // ��Ŷ ������ ����

                //        activeBlock2->m_fxPosition = meshPacket->m_fxPosition;
                //        activeBlock2->m_fyPosition = meshPacket->m_fyPosition;
                //        scene->m_nIndex2 = meshPacket->meshIndex;

                //        scene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, scene->m_pPlayer2);	//�ش� �ε����� ���� ����
                //        b2Vec2 velocity(0.0f, -50.0f);
                //        scene->m_objects2[scene->m_nIndexPlayerTwo]->m_pBody->SetLinearVelocity(velocity);




                //        /*std::string debugMessage = "Sending Packet: fx: " + std::to_string(activeBlock->m_fxPosition) +
                //            ", fy: " + std::to_string(activeBlock->m_fyPosition) +
                //            ", meshIndex: " + std::to_string(scene->m_nIndex) +
                //            "ž ������: " + std::to_string(activeBlock->m_pBody->GetPosition().y);
                //        LogDebugOutput(debugMessage);*/
                //    }


                   
                    
                //}

                std::string debugMessage = "[Debug] Received Mesh Packet: Position("
                    + std::to_string(meshPacket->m_fxPosition) + ", "
                    + std::to_string(meshPacket->m_fyPosition) + "), MeshIndex: "
                    + std::to_string(meshPacket->meshIndex)
                    + ", TopYPosition: " + std::to_string(meshPacket->topPositionRate)
                    + "win: " + std::to_string(meshPacket->Win);
                LogDebugOutput(debugMessage);
                break;
            }
            case KEY_CONTROL_PACKET: {
                // KEY_CONTROL_PACKET ó��
                PlayerKeyControl* keyPacket = reinterpret_cast<PlayerKeyControl*>(buffer);

                //std::lock_guard<std::mutex> lock(packetMutex);

                // �ٿ�Ű�� ���� ����
                /*if (keyPacket->nMessageID == VK_DOWN) {
                    std::lock_guard<std::mutex> lock(packetMutex);

                    if (scene->m_bTriggerActive2) {
                        scene->m_nIndexPlayerTwo = scene->RunTimeBuildObject(scene->m_nIndex2, pPlayer2);
                        scene->m_bTriggerActive2 = false;

                        GameObject* activeBlock = scene->m_objects2[scene->m_nIndexPlayerTwo];
                        b2Vec2 velocity(0.0f, -50.0f);
                        activeBlock->m_pBody->SetLinearVelocity(velocity);
                    }
                }*/
                break;
            }
            default: {
                /*std::string debugMessage = "[Error] Unknown packet type received.";
                LogDebugOutput(debugMessage);*/
                break;
            }
            }
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
        Sleep(100); // CPU ��뷮�� ���߱� ���� �ణ�� ���
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
        //���� �غ��� ��.
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

