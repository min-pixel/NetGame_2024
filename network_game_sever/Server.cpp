#include "Common.h"
#include <string>
#include <chrono>
#include <thread>
#include <windows.h>  // HANDLE, CreateThread, etc.
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2
#define UDP_SERVERPORT 9500

// UDP Thread 함수
DWORD WINAPI UDP_thread(LPVOID param) {
    SOCKET udp_sock = (SOCKET)param;  // 전달된 UDP 소켓을 받아서 사용
    struct sockaddr_in udp_clientaddr;
    

    int udp_addrlen;
    char udp_buf[BUFSIZE + 1];
    int udp_retval;
    int udp_count = 0;
    int udp_client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int port1;
    // 클라이언트와 데이터 통신
    while (1) {
        // 데이터 받기
        udp_addrlen = sizeof(udp_clientaddr);
        udp_retval = recvfrom(udp_sock, udp_buf, BUFSIZE, 0,
            (struct sockaddr*)&udp_clientaddr, &udp_addrlen);

        if (udp_retval == SOCKET_ERROR) {
            printf("UDP recvfrom error\n");
            break;
        }

        // 클라이언트 주소 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &udp_clientaddr.sin_addr, addr, sizeof(addr));
        int client_port = ntohs(udp_clientaddr.sin_port);

        // 받은 데이터 출력
        udp_buf[udp_retval] = '\0';  // 받은 데이터에 NULL 종료 문자 추가
        printf("받았습니다 udp %s   주소 : %s 포트: %d\n", udp_buf, addr, ntohs(udp_clientaddr.sin_port));
        
        // 클라이언트 포트가 이미 존재하는지 확인

        bool isNewClient = true;
        for (int i = 0; i < udp_count; i++) {
            if (udp_client_sockets[i] == client_port) {
                isNewClient = false;
                break;
            }
        }

        // 새로운 클라이언트일 경우, 배열에 저장
        if (isNewClient && udp_count < MAX_CLIENT_COUNT) {
            udp_client_sockets[udp_count] = client_port;
            udp_count++;  // 새로운 클라이언트가 추가되면 카운트 증가
            printf("새 클라이언트 포트: %d, 소켓이 있습니다\n", client_port);
        }

        // 두 클라이언트가 모두 연결되었으면, 서로 데이터를 주고받음
        if (udp_count == 2) {
            // 포트 번호에 따라 메시지를 다른 클라이언트에게 전송
            if (client_port == udp_client_sockets[0]) {
                udp_clientaddr.sin_port = htons(udp_client_sockets[1]);
                sendto(udp_sock, udp_buf, udp_retval, 0, (struct sockaddr*)&udp_clientaddr, sizeof(udp_clientaddr));
                printf("보냈습니다1 %s\n",udp_buf);
            }
            else if (client_port == udp_client_sockets[1]) {
                udp_clientaddr.sin_port = htons(udp_client_sockets[0]);
                sendto(udp_sock, udp_buf, udp_retval, 0, (struct sockaddr*)&udp_clientaddr, sizeof(udp_clientaddr));
                printf("보냈습니다2 %s\n", udp_buf);
            }
        }

        fflush(stdout);  // 출력 버퍼를 즉시 플러시

        


        
    }

    return 0;  // 쓰레드 종료
}

// Timer 함수 (기존처럼 경과 시간 계산 및 전송)
DWORD WINAPI Timer(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // 클라이언트 소켓 배열
    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // 시작 시간 기록
    int retval;

    while (true) {
        // 경과 시간 계산
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float 형식으로 경과 시간 저장
        inner_time = static_cast<int>(elapsed.count()); // 경과 시간을 int로 변환

        // 모든 소켓에 경과 시간 전송
        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
            
        }

        printf("보낸 경과시간: %d초\n", inner_time); // 서버에서 경과 시간 출력

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 데이터 너무 많이 가는 문제 방지
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int retval;
    int udp_retval;
    WSADATA wsa;

    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // TCP 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) err_quit("socket()");

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) err_quit("bind()");

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");

    // UDP 소켓 생성
    SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock == INVALID_SOCKET) err_quit("socket()");

    printf("UDP소켓연결성공\n");

    // UDP bind()
    struct sockaddr_in udp_serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    udp_serveraddr.sin_family = AF_INET;
    udp_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_serveraddr.sin_port = htons(UDP_SERVERPORT);
    udp_retval = bind(udp_sock, (struct sockaddr*)&udp_serveraddr, sizeof(udp_serveraddr));
    if (udp_retval == SOCKET_ERROR) err_quit("bind()");



    // 클라이언트 소켓 배열
    SOCKET client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int client_count = 0;

    // 클라이언트 연결 처리
    while (client_count < MAX_CLIENT_COUNT) {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET client_socket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        

        // 클라이언트 소켓 배열에 저장
        client_sockets[client_count] = client_socket;

        // 접속한 클라이언트 정보 출력
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr.sin_port));

        client_count++; // 클라이언트 카운트 증가
        

        // 클라이언트 1 대기 메시지 출력
        if (client_count == 1) {
            printf("클라이언트 1 접속 완료. 클라이언트 2를 기다립니다...\n");
        }
    }

    // 모든 클라이언트 연결 완료
    printf("\n클라이언트 2 접속 완료. 게임을 시작합니다.\n");

    // Timer 쓰레드 실행
    HANDLE timer_thread = CreateThread(NULL, 0, Timer, (LPVOID)client_sockets, 0, NULL);
    HANDLE udp_thread = CreateThread(NULL, 0, UDP_thread, (LPVOID)udp_sock, 0, NULL);

    // 쓰레드 종료 대기
    WaitForSingleObject(timer_thread, INFINITE);
    WaitForSingleObject(udp_thread, INFINITE);
    CloseHandle(timer_thread);
    CloseHandle(udp_thread);
    // 소켓 닫기
    for (int i = 0; i < client_count; i++) {
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}