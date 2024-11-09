#pragma once

#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include "Packet.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFFER_LENGTH 512 // 버퍼의 크기를 정의


// 전방 선언 추가
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
    bool SendData(void* packet);  // 패킷 전송 함수 추가
    void Client_Send_Thread(Player* pPlayer, Scene* scene);
    //bool ReceiveData(std::string& outData); // 서버로부터 데이터 수신
    void Disconnect();

    SOCKET GetSocket() const { return m_socket; }

private:
    SOCKET m_socket;
    bool m_initialized;
};

