#include "NetworkManager.h"
#include "Packet.h"
#include "Scene.h"
#include "GameObject.h"
#include <thread>
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

    return true;
}

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
}

//--------------------------------------------------------------------------------------------------------여기서부터 구현 (11/09)
bool NetworkManager::SendData(void* packet) {
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Socket is not connected." << std::endl;
        return false;
    }

    // 패킷 타입을 먼저 전송하고, 그 후 패킷 데이터를 전송
    PacketType packetType = *(PacketType*)packet;
    int result;

    // 패킷 타입을 서버에 전송
    result = send(m_socket, (char*)&packetType, sizeof(PacketType), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed (packet type): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }

    // 패킷 타입에 따라 다른 구조체 데이터를 전송
    switch (packetType) {
    case KEY_CONTROL_PACKET: {
        PlayerKeyControl* keyControlPacket = static_cast<PlayerKeyControl*>(packet);
        result = send(m_socket, (char*)keyControlPacket, sizeof(PlayerKeyControl), 0);
        break;
    }
    default:
        std::cerr << "Unknown packet type." << std::endl;
        return false;
    }

    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed (packet data): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }

    std::cout << "Packet Sent: " << result << " bytes." << std::endl;
    return true;
}


void NetworkManager::Client_Send_Thread(Player* player, Scene* scene) {
    
    PlayerKeyControl keyControlPacket; // 키 입력 제어 패킷

    while (scene->m_bGameStop == false) {  // scene의 m_bGameStop을 사용하여 루프 유지 여부 결정

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
            this->SendData(static_cast<void*>(&keyControlPacket)); 
        }
        Sleep(100); // 전송 주기 (0.1초)
    }
}

