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
#define BUFSIZE 512

//아직 미완인 부분은 주석 처리함.

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

    std::cout << "Connected to server!" << std::endl;

    // UDP 소켓 생성
    udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket == INVALID_SOCKET)
    {
        std::cerr << "UDP socket creation failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        WSACleanup();
        return false;
    }
    std::cout << "UDP socket created!" << std::endl;

    // UDP 소켓에 서버 주소 바인딩 (필요 시)
    result = connect(udpSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::cerr << "UDP connection setup failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        closesocket(udpSocket);
        return false;
    }
    std::cout << "UDP socket setup completed!" << std::endl;


    // 서버로 간단한 테스트 데이터 전송
    const char* testData = "Connection test message from game client";
    result = send(m_socket, testData, static_cast<int>(strlen(testData)), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "Sent test data: " << testData << std::endl;

    // getPlayer 패킷 생성 및 서버로 전송
    //getPlayer playerPacket;
    //playerPacket.packetType = GET_PLAYER_PACKET;
    //playerPacket.ip = serverIP;           // 클라이언트 IP 설정
    //playerPacket.nMessageID = 1;          // 예시로 ID 1을 사용
    //playerPacket.accept_server = true;    // 서버 승인 여부 설정

    //// getPlayer 패킷 전송
    //result = send(m_socket, (char*)&playerPacket, sizeof(getPlayer), 0);
    //if (result == SOCKET_ERROR)
    //{
    //    std::cerr << "Send failed (getPlayer packet): " << WSAGetLastError() << std::endl;
    //    closesocket(m_socket);
    //    return false;
    //}
    //std::cout << "Sent getPlayer packet to server." << std::endl;

    //// 데이터 수신 대기 루프
    //char recvbuf[BUFSIZE];
    //while (true) {
    //    result = recv(m_socket, recvbuf, BUFSIZE, 0);
    //    if (result > 0) {
    //        recvbuf[result] = '\0';
    //        std::cout << "[Received Data] " << recvbuf << std::endl;
    //    }
    //    else if (result == 0) {
    //        std::cout << "Connection closed by server." << std::endl;
    //        break;
    //    }
    //    else {
    //        std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
    //        break;
    //    }
    //}

    // 서버 정보 저장 (멤버 변수)
    m_serverIP = serverIP;
    m_serverPort = serverPort;

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
////--------------------------------------------------------------메세지를 보내는 거 뿐만 아니라, 데이터를 보내는 send함수가 필요 
//// 서용처 : void Scene::BuildObjects()에서 m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);룰 보내야 함
//
////--------------------------------------------------------------서버에서 보내는 데이터를 받을 리시브 함수가 필요함
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

    if (udpSocket != INVALID_SOCKET)
    {
        closesocket(udpSocket);
        udpSocket = INVALID_SOCKET;
    }
}

//--------------------------------------------------------------------------------------------------------여기서부터 구현 (11/09)
bool NetworkManager::SendData(void* packet) {

    PacketType packetType = *(PacketType*)packet;
    int result;

    // 패킷 타입 전송
    result = send(m_socket, (char*)&packetType, sizeof(PacketType), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "[Error] Send failed (packet type): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "[Debug] Sent packet type: " << packetType << std::endl;

    // 패킷 데이터 전송
    switch (packetType) {
    case KEY_CONTROL_PACKET: {
        PlayerKeyControl* keyControlPacket = static_cast<PlayerKeyControl*>(packet);
        result = send(m_socket, (char*)keyControlPacket, sizeof(PlayerKeyControl), 0);
        break;
    }
    case MESH_PACKET: {
        // 메쉬 패킷은 UDP로 전송
        PlayerMeshPacket* meshPacket = static_cast<PlayerMeshPacket*>(packet);


        std::string debugMessage = "Packet Data - Type: MESH_PACKET, fx: " +
            std::to_string(meshPacket->m_fxPosition) +
            ", fy: " + std::to_string(meshPacket->m_fyPosition) +
            ", meshIndex: " + std::to_string(meshPacket->meshIndex);
        LogDebugOutput(debugMessage);

        sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(9500); // 서버 포트 설정
        inet_pton(AF_INET, m_serverIP.c_str(), &serverAddr.sin_addr);

        // UDP 전송 : 문자열로 보내야 함. 혹은 배열값으로
        result = sendto(udpSocket, (char*)meshPacket, sizeof(PlayerMeshPacket), 0,
            (sockaddr*)&serverAddr, sizeof(serverAddr));
        if (result == SOCKET_ERROR)
        {
            std::cerr << "[Error] UDP send failed: " << WSAGetLastError() << std::endl;
        }
        else
        {
            std::cout << "[Debug] Sent Mesh Packet via UDP (Index: " << meshPacket->meshIndex << ")" << std::endl;
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
    
    PlayerMeshPacket meshPacket = {};  // 모든 값을 0으로 초기화 
    PlayerKeyControl keyControlPacket = {}; // 모든 값을 0으로 초기화 

    
    //meshPacket.packetType = MESH_PACKET;

    // 이전 좌표를 저장하는 변수
    float lastFxPosition = 0.0f;
    float lastFyPosition = 0.0f;
    
    //클라에서 데이터가 제대로 보내지는지 확인, 똑바로 가져와지는 지 확인.........

    while (scene->m_bGameStop == false) {  // scene의 m_bGameStop을 사용하여 루프 유지 여부 결정
        std::lock_guard<std::mutex> lock(packetMutex);
        if (scene->m_pNextGameObjectOne) {

            if (!scene->m_objects.empty() && scene->m_nIndexPlayerOne >= 0 &&
                scene->m_nIndexPlayerOne < scene->m_objects.size()) {

                GameObject* activeBlock = scene->m_objects[scene->m_nIndexPlayerOne];  //다음 블럭의 위치 정보가 아니라, 현재 조작 중인 블럭이어야 함. 즉, 수정이 필요.
                // 패킷 데이터 설정

                //if (activeBlock->m_fxPosition != lastFxPosition || activeBlock->m_fyPosition != lastFyPosition) {
                    //lastFxPosition = activeBlock->m_fxPosition;
                    //lastFyPosition = activeBlock->m_fyPosition;

                meshPacket.packetType = MESH_PACKET;
                meshPacket.m_fxPosition = activeBlock->m_fxPosition;
                meshPacket.m_fyPosition = activeBlock->m_fyPosition;
                meshPacket.meshIndex = scene->m_nIndex;

                // 서버로 패킷 전송
                this->SendData(&meshPacket);
                if (!this->SendData(&meshPacket)) {
                    std::cerr << "[Error] Failed to send mesh packet to server." << std::endl;
                }


                /*std::string debugMessage = "Sending Packet: fx: " + std::to_string(activeBlock->m_fxPosition) +
                    ", fy: " + std::to_string(activeBlock->m_fyPosition) +
                    ", meshIndex: " + std::to_string(scene->m_nIndex);
                LogDebugOutput(debugMessage);*/



            }

           // }
        }
        

        // 서버로 패킷 전송
        //this->SendData(&meshPacket);


        // 키 입력이 있을 경우 키 입력 제어 패킷 전송
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

            // 디버깅 메시지
            std::cout << "[Debug] Sending Key Packet: Key = " << keyControlPacket.nMessageID << std::endl;

            keyControlPacket.packetType = KEY_CONTROL_PACKET;
            //meshPacket.meshIndex = scene->m_nIndex;
            this->SendData(static_cast<void*>(&keyControlPacket));
            //this->SendData(&meshPacket);

            // 플레이어 1의 조작 중인 블록의 위치 정보 전송
            //if (scene->m_pNextGameObjectOne) {
            //    GameObject* activeBlock = scene->m_pNextGameObjectOne;

            //    // 이전 위치와 비교하여 변경이 있는 경우에만 전송
            //    if (activeBlock->m_fxPosition != lastFxPosition || activeBlock->m_fyPosition != lastFyPosition) {
            //        // 좌표 업데이트
            //        lastFxPosition = activeBlock->m_fxPosition;
            //        lastFyPosition = activeBlock->m_fyPosition;

            //        // 패킷 데이터 설정
            //        meshPacket.packetType = MESH_PACKET;
            //        meshPacket.m_fxPosition = activeBlock->m_fxPosition;
            //        meshPacket.m_fyPosition = activeBlock->m_fyPosition;
            //        meshPacket.meshIndex = scene->m_nIndex;

            //        // 디버깅 메시지
            //        std::cout << "[Debug] Sending Mesh Packet: ("
            //            << meshPacket.m_fxPosition << ", "
            //            << meshPacket.m_fyPosition << ")" << std::endl;

            //        // 서버로 패킷 전송
            //        this->SendData(&meshPacket);
            //    }
            //}



        }
        Sleep(1000); // 전송 주기 (1초)
    
    }
}




