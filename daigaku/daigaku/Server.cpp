#include "Packet.h"
// HANDLE, CreateThread, etc.
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2
#define UDP_SERVERPORT 9500


DWORD WINAPI Combined_TCP_Thread(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // 클라이언트 소켓 배열
    char buffer[BUFSIZE + 1];
    int retval;
    int inner_time;

    auto start = std::chrono::high_resolution_clock::now(); // 시작 시간 기록

    fd_set readfds;  // 읽기 가능한 소켓을 관리할 집합 //이부분 select GPT사용 좀더 해봐야할듯 데이터 안들어올때 오류뜨는거라

    while (true) {
        // 경과 시간 계산
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float 형식으로 경과 시간 저장
        inner_time = static_cast<int>(elapsed.count());

        // 읽기 가능한 소켓을 확인
        FD_ZERO(&readfds);  // 기존에 설정된 소켓 집합 초기화
        FD_SET(client_sockets[0], &readfds);  // 첫 번째 클라이언트 소켓을 추가
        FD_SET(client_sockets[1], &readfds);  // 두 번째 클라이언트 소켓을 추가

        // select 호출하여 읽기 가능한 소켓을 기다림 (타임아웃을 0.1초로 설정)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        retval = select(0, &readfds, NULL, NULL, &timeout);
        if (retval == SOCKET_ERROR) {
            printf("select error\n");
            break;
        }

        if (retval == 0) {
            // 타임아웃 발생 (데이터가 없으면 계속 대기)
            continue;
        }

        // 데이터를 수신할 수 있는 소켓 확인
        for (int i = 0; i < 2; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {  // 해당 소켓이 준비되었을 때만 처리
                // 데이터 수신
                retval = recv(client_sockets[i], buffer, BUFSIZE, 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP recv error (클라이언트 %d)\n", i);
                    continue; // 에러가 발생한 경우 이 클라이언트는 건너뛰고, 다음 클라이언트로 이동
                }

                if (retval == 0) {
                    // 클라이언트가 연결을 종료한 경우
                    printf("클라이언트 %d가 연결을 종료했습니다.\n", i);
                    continue; // 연결 종료된 클라이언트는 처리하지 않고 넘어갑니다.
                }

                // 받은 데이터 처리
                PlayerMeshPacket* receivedPacket = (PlayerMeshPacket*)buffer;

                // 받은 데이터 출력
                printf("받은 데이터(클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                    i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

                // 상대 클라이언트로 데이터 전송
                int targetClient = (i == 0) ? 1 : 0;  // 현재 클라이언트가 0이면 1로
                retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP send error (클라이언트 %d -> 클라이언트 %d)\n", i, targetClient);
                    continue; // 전송 오류가 발생한 경우, 해당 전송을 건너뛰고 계속 진행
                }
               
                printf("보냈습니다(클라이언트 %d -> 클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                    i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
            }
        }

        // 경과 시간을 1초마다 모든 클라이언트에 전송
        for (int i = 0; i < 2; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);

            if (retval == SOCKET_ERROR) {
                printf("TCP send time error (클라이언트 %d)\n", i);
                continue; // 시간 전송 오류가 발생하면 해당 클라이언트는 건너뛰고 계속 진행
            }
        }
        if (inner_time == 100)
        {
            for (int i = 0; i < 2; i++)
            {
                retval = send(client_sockets[i], /**/);
                if (retval == SOCKET_ERROR)
                {
                    printf("sendERROR %d\n", i);
                }
            }
            //소켓종료부분
           

        }

        printf("시간 보냈음 %d\n", inner_time);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // 잠시 대기 후 계속 처리
    }

    return 0;  // 쓰레드 종료
}
//tcp 패킷 관련
DWORD WINAPI ITEM_KEY_Packet(LPVOID param)
{
    SOCKET* client_sockets = (SOCKET*)param;
    char key_buf[BUFSIZE + 1];
    char item_buf[BUFSIZE + 1];
    int retval;
    PacketType packetTp;
    PlayerKeyControlPacket* recvKeyPacket = (PlayerKeyControlPacket*)key_buf;
    PlayerItemPacket* recvItemPacket = (PlayerItemPacket*)item_buf;
    while (1)
    {
        for (int i = 0; i < MAX_CLIENT_COUNT; i++)
        {
            retval = recv(client_sockets[i], key_buf, sizeof(client_sockets), 0);
            if (SOCKET_ERROR == retval)
            {
                err_quit("recvErrorKey");
                continue;
            }
        }
        //recv Key 
        switch (packetTp) {
        case KEY_CONTROL_PACKET:
        { 
            //send key
            for (int i = 0; i < MAX_CLIENT_COUNT; i++)
            {
                retval = send(client_sockets[i], key_buf, sizeof(PlayerKeyControlPacket), 0);
                if (SOCKET_ERROR == retval)
                {
                    err_quit("SendErrorKey");
                    continue;
                }
            }

        }
        break;
        case ITEM_PACKET:
        {
            for (int i = 0; i < MAX_CLIENT_COUNT; i++)
            {
                retval = send(client_sockets[i], key_buf, sizeof(PlayerItemPacket), 0);
                if (SOCKET_ERROR == retval)
                {
                    err_quit("SendErrorKey");
                    continue;
                }
            }
        }
        break;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int retval;
    int udp_retval;
    WSADATA wsa;

    HANDLE TcpThread;

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
    HANDLE timer_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);
    TcpThread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    //player key recv

    // 쓰레드 종료 대기
    WaitForSingleObject(Combined_TCP_Thread, INFINITE);
    WaitForSingleObject(ITEM_KEY_Packet, INFINITE);
    CloseHandle(TcpThread);
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