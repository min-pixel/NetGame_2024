#include "Packet.h"


char* SERVERIP = (char*)"127.0.0.2";
#define SERVERPORT 9000
#define BUFSIZE 512
#define WM_LBUTTONDOWN 0x0201  // 서버 요구 메시지 ID 값
#define MAX_CLIENT_COUNT 2



DWORD WINAPI Combined_TCP_Thread(LPVOID param) {//TCP 부분 아직 키값은 안함 키값 어케해야할지 애매함
    SOCKET* client_sockets = (SOCKET*)param;  // 클라이언트 소켓 배열
    char buffer[BUFSIZE + 1];
    int retval;

    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // 시작 시간 기록

    while (true) {

        // 경과 시간 계산
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float 형식으로 경과 시간 저장
         // 경과 시간을 int로 변환
        inner_time = static_cast<int>(elapsed.count());
        // 데이터를 받은 후, 어느 소켓에서 왔는지 확인
        for (int i = 0; i < 2; i++) {
            // 데이터 수신
            retval = recv(client_sockets[i], buffer, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP recv error\n");
                break;
            }

            PlayerMeshPacket* receivedPacket = (PlayerMeshPacket*)buffer;

            // 받은 데이터 출력
            printf("받은 데이터(클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

            int targetClient = (i == 0) ? 1 : 0;  // 현재 소켓이 0이면 1번으로, 1이면 0번으로
            retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP send error\n");
                break;
            }

            printf("보냈습니다(클라이언트 %d -> 클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
        }
        
        // 경과 시간을 1초마다 모든 클라이언트에 전송 (예시)
        for (int i = 0; i < 2; i++) {
              // 예시로 임의의 시간값 (경과 시간 등)
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
           
            if (retval == SOCKET_ERROR) {
                printf("TCP send time error\n");
                break;
            }
        }
        printf("시간 보냈음 %d", inner_time);
        
        
    }

    return 0;  // 쓰레드 종료
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
    bool start_btn = true;
    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");







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

    for (int i = 0; i < 2; i++) {
        
        retval = send(client_sockets[i], reinterpret_cast<char*>(&start_btn), sizeof(start_btn), 0);

        if (retval == SOCKET_ERROR) {
            printf("TCP send time error\n");
            break;
        }
    }
    printf("시작값 보냄\n");
    // Timer 쓰레드 실행
    HANDLE timer_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);


    // 쓰레드 종료 대기
    WaitForSingleObject(timer_thread, INFINITE);

    CloseHandle(timer_thread);

    // 소켓 닫기
    for (int i = 0; i < client_count; i++) {
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}