#include "NetworkManager.h"
#include "Packet.h"
#include "Scene.h"
#include "GameObject.h"
#include <thread>

#define BUFSIZE 512

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

    std::cout << "Connected to server!" << std::endl;

    // ������ ������ �׽�Ʈ ������ ����
    const char* testData = "Connection test message from game client";
    result = send(m_socket, testData, static_cast<int>(strlen(testData)), 0);
    if (result == SOCKET_ERROR)
    {
        std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "Sent test data: " << testData << std::endl;

    // getPlayer ��Ŷ ���� �� ������ ����
    //getPlayer playerPacket;
    //playerPacket.packetType = GET_PLAYER_PACKET;
    //playerPacket.ip = serverIP;           // Ŭ���̾�Ʈ IP ����
    //playerPacket.nMessageID = 1;          // ���÷� ID 1�� ���
    //playerPacket.accept_server = true;    // ���� ���� ���� ����

    //// getPlayer ��Ŷ ����
    //result = send(m_socket, (char*)&playerPacket, sizeof(getPlayer), 0);
    //if (result == SOCKET_ERROR)
    //{
    //    std::cerr << "Send failed (getPlayer packet): " << WSAGetLastError() << std::endl;
    //    closesocket(m_socket);
    //    return false;
    //}
    //std::cout << "Sent getPlayer packet to server." << std::endl;

    //// ������ ���� ��� ����
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
    if (m_socket == INVALID_SOCKET) {
        std::cerr << "[Error] Socket is not connected." << std::endl;
        return false;
    }

    PacketType packetType = *(PacketType*)packet;
    int result;

    // ��Ŷ Ÿ�� ����
    result = send(m_socket, (char*)&packetType, sizeof(PacketType), 0);
    if (result == SOCKET_ERROR) {
        std::cerr << "[Error] Send failed (packet type): " << WSAGetLastError() << std::endl;
        closesocket(m_socket);
        return false;
    }
    std::cout << "[Debug] Sent packet type: " << packetType << std::endl;

    // ��Ŷ ������ ����
    switch (packetType) {
    case KEY_CONTROL_PACKET: {
        PlayerKeyControl* keyControlPacket = static_cast<PlayerKeyControl*>(packet);
        result = send(m_socket, (char*)keyControlPacket, sizeof(PlayerKeyControl), 0);
        break;
    }
    case MESH_PACKET: {
        PlayerMeshPacket* meshPacket = static_cast<PlayerMeshPacket*>(packet);
        result = send(m_socket, (char*)meshPacket, sizeof(PlayerMeshPacket), 0);
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
    
    PlayerKeyControl keyControlPacket; // Ű �Է� ���� ��Ŷ

    PlayerMeshPacket meshPacket;

    //meshPacket.packetType = MESH_PACKET;

    // ���� ��ǥ�� �����ϴ� ����
    //float lastFxPosition = -1.0f;
    //float lastFyPosition = -1.0f;
    


    while (scene->m_bGameStop == false) {  // scene�� m_bGameStop�� ����Ͽ� ���� ���� ���� ����

        //���� ��ġ ���� ��Ŷ ���� (�̿�)
        //for (auto& object : scene->m_objects) {
        //    if (object->m_fxPosition != lastFxPosition || object->m_fyPosition != lastFyPosition) { // y ��ǥ�� 0���� ū ��ϸ� ����
        //        
        //        // ��ǥ ������Ʈ
        //        lastFxPosition = object->m_fxPosition; 
        //        lastFyPosition = object->m_fyPosition; 
        //        
        //        meshPacket.m_fxPosition = object->m_fxPosition;
        //        meshPacket.m_fyPosition = object->m_fyPosition;

        //        // ����� �޽���
        //        std::cout << "[Debug] Sending Mesh Packet: ("
        //            << meshPacket.m_fxPosition << ", "
        //            << meshPacket.m_fyPosition << ")" << std::endl;

        //        // ������ ��Ŷ ����
        //        this->SendData(&meshPacket);
        //    }
        //}


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
                std::cout << "[Debug] Sending Key Packet: Key = " << keyControlPacket.nMessageID << std::endl;

                meshPacket.meshIndex = scene->m_nIndex;
                this->SendData(static_cast<void*>(&keyControlPacket));
                this->SendData(&meshPacket);
            }
            Sleep(100); // ���� �ֱ� (0.1��)
        
    }
}

