#include "NetworkManager.h"
#include "Packet.h"
#include "Scene.h"
#include "GameObject.h"
#include <thread>
<<<<<<< HEAD
=======
#include <iostream>
#include <string>

#define BUFSIZE    512

>>>>>>> 4577a03 (ì„œë²„ì™€ ì—°ê²° í™•ì¸ (1ì°¨))
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

<<<<<<< HEAD
    return true;
}

=======
    std::cout << "Connected to server!" << std::endl;

    // ¼­¹ö·Î °£´ÜÇÑ Å×½ºÆ® µ¥ÀÌÅÍ Àü¼Û
    const char* testData = "Connection test message from game client";
    result = send(m_socket, testData, static_cast<int>(strlen(testData)), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "Sent test data: " << testData << std::endl;

    // µ¥ÀÌÅÍ ¼ö½Å ´ë±â ·çÇÁ
    char recvbuf[BUFSIZE];
    while (true) {
        result = recv(m_socket, recvbuf, BUFSIZE, 0);
        if (result > 0) {
            recvbuf[result] = '\0';
            std::cout << "[Received Data] " << recvbuf << std::endl;
        }
        else if (result == 0) {
            std::cout << "Connection closed by server." << std::endl;
            break;
        }
        else {
            std::cerr << "Receive failed: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    return true;
}
>>>>>>> 4577a03 (ì„œë²„ì™€ ì—°ê²° í™•ì¸ (1ì°¨))
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
////--------------------------------------------------------------¸Ş¼¼Áö¸¦ º¸³»´Â °Å »Ó¸¸ ¾Æ´Ï¶ó, µ¥ÀÌÅÍ¸¦ º¸³»´Â sendÇÔ¼ö°¡ ÇÊ¿ä 
//// ¼­¿ëÃ³ : void Scene::BuildObjects()¿¡¼­ m_pNextGameObjectTwo = RandomMesh(m_pPlayer2);·ê º¸³»¾ß ÇÔ
//
////--------------------------------------------------------------¼­¹ö¿¡¼­ º¸³»´Â µ¥ÀÌÅÍ¸¦ ¹ŞÀ» ¸®½Ãºê ÇÔ¼ö°¡ ÇÊ¿äÇÔ
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

//--------------------------------------------------------------------------------------------------------¿©±â¼­ºÎÅÍ ±¸Çö (11/09)
bool NetworkManager::SendData(void* packet) {
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "Socket is not connected." << std::endl;
        return false;
    }

    // ÆĞÅ¶ Å¸ÀÔÀ» ¸ÕÀú Àü¼ÛÇÏ°í, ±× ÈÄ ÆĞÅ¶ µ¥ÀÌÅÍ¸¦ Àü¼Û
    PacketType packetType = *(PacketType*)packet;
    int result;

    // ÆĞÅ¶ Å¸ÀÔÀ» ¼­¹ö¿¡ Àü¼Û
    result = send(m_socket, (char*)&packetType, sizeof(PacketType), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "Send failed (packet type): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }

    // ÆĞÅ¶ Å¸ÀÔ¿¡ µû¶ó ´Ù¸¥ ±¸Á¶Ã¼ µ¥ÀÌÅÍ¸¦ Àü¼Û
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
    
    PlayerKeyControl keyControlPacket; // Å° ÀÔ·Â Á¦¾î ÆĞÅ¶

<<<<<<< HEAD
=======
    getPlayer getplayinfo;

    


>>>>>>> 4577a03 (ì„œë²„ì™€ ì—°ê²° í™•ì¸ (1ì°¨))
    while (scene->m_bGameStop == false) {  // sceneÀÇ m_bGameStopÀ» »ç¿ëÇÏ¿© ·çÇÁ À¯Áö ¿©ºÎ °áÁ¤

        // Å° ÀÔ·ÂÀÌ ÀÖÀ» °æ¿ì Å° ÀÔ·Â Á¦¾î ÆĞÅ¶ Àü¼Û
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
        Sleep(100); // Àü¼Û ÁÖ±â (0.1ÃÊ)
    }
}

