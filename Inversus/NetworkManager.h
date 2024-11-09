#pragma once

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Packet.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LENGTH 512 // ������ ũ�⸦ ����


// ���� ���� �߰�
class Scene;
class Player;




class NetworkManager
{
public:
    NetworkManager();
    ~NetworkManager();

    bool Initialize();
    bool ConnectToServer(const std::string& serverIP, int serverPort);
    //bool SendMessageFunc(const std::string& data);
    bool SendData(void* packet);  // ��Ŷ ���� �Լ� �߰�
    void Client_Send_Thread(Player* pPlayer, Scene* scene);
    //bool ReceiveData(std::string& outData); // �����κ��� ������ ����
    void Disconnect();

    SOCKET GetSocket() const { return m_socket; }

private:
    SOCKET m_socket;
    bool m_initialized;
};

