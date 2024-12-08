#include "Packet.h"


char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 1024
#define WM_LBUTTONDOWN 0x0201  // 서버 요구 메시지 ID 값
#define MAX_CLIENT_COUNT 2




DWORD WINAPI Combined_TCP_Thread(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // 클라이언트 소켓 배열
    char buffer[BUFSIZE + 1];
    int retval;
    int inner_time = 0;
    int SavePosition[2]{};
    auto start = std::chrono::high_resolution_clock::now(); // 시작 시간 기록
    const char* win_game = "이겼습니다";
    const char* lose_game = "졌습니다";
    fd_set readfds;  // 읽기 가능한 소켓을 관리할 집합

    while (true) {
        // 경과 시간 계산
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float 형식으로 경과 시간 저장
        inner_time = static_cast<int>(elapsed.count());

        

        for (int i = 0; i < 2; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP send time error (클라이언트 %d)\n", i);
                continue; // 시간 전송 오류가 발생하면 해당 클라이언트는 건너뛰고 계속 진행
            }
        }
        printf("시간 보냈음 %d\n", inner_time);

        // 읽기 가능한 소켓을 확인
        FD_ZERO(&readfds);  // 기존에 설정된 소켓 집합 초기화
        FD_SET(client_sockets[0], &readfds);  // 첫 번째 클라이언트 소켓을 추가
        FD_SET(client_sockets[1], &readfds);  // 두 번째 클라이언트 소켓을 추가

        // select 호출하여 읽기 가능한 소켓을 기다림 (타임아웃을 0.1초로 설정)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;  // 0.2초 타임아웃

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

                // Position 데이터를 저장
                SavePosition[i] = receivedPacket->topPositionRate;

                // 받은 데이터 출력
                printf("받은 데이터(클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                    i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

                // 상대 클라이언트로 데이터 전송
                int targetClient = (i == 0) ? 1 : 0;  // 현재 클라이언트가 0이면 1로, 1이면 0으로 전송
                retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP send error (클라이언트 %d -> 클라이언트 %d)\n", i, targetClient);
                    
                }

                printf("보냈습니다(클라이언트 %d -> 클라이언트 %d): Position(%f, %f), Mesh Index:%d\n",
                    i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
            }
        }
        
        
        if (inner_time == 100)
        {
            //일단 이부분 클라에서 최대 높이값 받아와야 검증 가능

            for (int i = 0; i < 2; i++)
            {
                PlayerMeshPacket meshHeight;
                if (SavePosition[0] < SavePosition[1])
                {
                    //meshHeight.Win = (i == 1);//이부분 생각좀 해봐야할듯 우선 클라2개 bool값 보내고  문자열로 승리 패배 보내주면 그거 출력하게
                    for (int i = 0; i < 2; i++) {
                        retval = send(client_sockets[i], (char*)&meshHeight, sizeof(PlayerMeshPacket), 0);
                    }
                    retval = send(client_sockets[1], (char*)&win_game, sizeof(win_game), 0);
                    retval = send(client_sockets[0], (char*)&lose_game, sizeof(win_game), 0);
                }
                else if (SavePosition[1] < SavePosition[0]) {
                    //meshHeight.Win = (i == 0);
                    for (int i = 0; i < 2; i++) {
                        retval = send(client_sockets[i], (char*)&meshHeight, sizeof(PlayerMeshPacket), 0);
                    }
                    retval = send(client_sockets[0], (char*)&win_game, sizeof(win_game), 0);
                    retval = send(client_sockets[1], (char*)&lose_game, sizeof(win_game), 0);
                }

                //retval = send(client_sockets[i], (char*)&meshHeight, sizeof(PlayerMeshPacket), 0); 이부분 왜?
                //소켓 값에 동일하게 종료 되었다란 걸 보내야 할텐데
                if (retval == SOCKET_ERROR)
                {
                    printf("sendERROR %d\n", i);
                }
            }
            break;
            //소켓종료부분
            

        }
        // 이부분 클라랑 맞춰야함(초당 주고받는 패킷)
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        
    }

    // 소켓 종료 처리
    for (int i = 0; i < 2; i++) {
        closesocket(client_sockets[i]);
    }

    return 0;  // 쓰레드 종료
}






DWORD WINAPI ITEM_KEY_Packet(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;

    char recv_bufs[MAX_CLIENT_COUNT][BUFSIZE];
    int retval;
   
    while (1) {

        

        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            memset(recv_bufs[i], 0, BUFSIZE);
            retval = recv(client_sockets[i], recv_bufs[i], sizeof(PlayerPacket), 0);
            if (SOCKET_ERROR == retval) {
                printf("recv 에러 %d번째 소켓: %d\n", i, WSAGetLastError());
                continue;
            }

            PlayerPacket* pk = (PlayerPacket*)recv_bufs[i];
            retval = recv(client_sockets[i], recv_bufs[i] + sizeof(PlayerPacket), pk->size - sizeof(PlayerPacket), 0);
            if (SOCKET_ERROR == retval) {
                printf("recv 에러 %d번째 소켓: %d\n", i, WSAGetLastError());
                continue;
            }

            switch (pk->packetType) {
            case KEY_CONTROL_PACKET: {
                PlayerKeyControlPacket* recvKeyPacket = (PlayerKeyControlPacket*)recv_bufs[i];
                printf("클라이언트 %d에서 키 데이터 수신: %d\n", i, recvKeyPacket->nMessageID);

                // 키 데이터 처리 후 전송
                retval = send(client_sockets[i], (char*)recvKeyPacket, sizeof(PlayerKeyControlPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("send 에러 %d번째 소켓: %d\n", i, WSAGetLastError());
                }
                printf("키값 보냈습니다 %d\n", recvKeyPacket->nMessageID);
                break;
            }
            case ITEM_PACKET: {
                PlayerItemPacket* recvItemPacket = (PlayerItemPacket*)recv_bufs[i];
                printf("클라이언트 %d에서 아이템 데이터 수신: %d\n", i, recvItemPacket->m_nChangeBlockCount);

                // 아이템 데이터 처리 후 전송
                retval = send(client_sockets[i], (char*)recvItemPacket, sizeof(PlayerItemPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("send 에러 %d번째 소켓: %d\n", i, WSAGetLastError());
                }
                break;
            }
            
           
            }
        }
    }
    return 0;
}


// 게임 시작 신호 수신 확인 배열
bool start_signals[MAX_CLIENT_COUNT] = { false, false };

int main(int argc, char* argv[]) {
    int retval;
    WSADATA wsa;

    // 윈속 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup 오류: %d\n", WSAGetLastError());
        return 1;
    }

    // TCP 소켓 생성
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        printf("소켓 생성 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // bind()
    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVERPORT);
    retval = bind(listen_sock, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if (retval == SOCKET_ERROR) {
        printf("bind() 실패: %d\n", WSAGetLastError());
        return 1;
    }

    bool start_btn = true;

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        printf("listen() 실패: %d\n", WSAGetLastError());
        return 1;
    }

    // 클라이언트 소켓 배열
    SOCKET client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int client_count = 0;

    // 클라이언트 연결 처리
    while (client_count < MAX_CLIENT_COUNT) {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET client_socket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_socket == INVALID_SOCKET) {
            printf("accept() 실패: %d\n", WSAGetLastError());
            continue;
        }

        // 클라이언트 소켓 배열에 저장
        client_sockets[client_count] = client_socket;
        getPlayer myPlayer;
        myPlayer.count = htonl(client_count);                                             //카운트 값 받기 위해 수정 12/08   
        retval = send(client_sockets[client_count], reinterpret_cast<char*>(&myPlayer.count), sizeof(myPlayer.count), 0); // 카운트 값 받기 위해 수정 12/08
        if (retval == SOCKET_ERROR) {
            printf("send() failed: %d\n", WSAGetLastError());
        }
        else {
            printf("Sent player count: %d\n", client_count);
        }
        printf("%d번째 플레이어인지 보냄", myPlayer.count);

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

    // 스타트 신호 수신 루프  ------------------------------------------------12/08
    while (true) {
        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            char buffer[sizeof(bool)] = { 0 };
            retval = recv(client_sockets[i], buffer, sizeof(bool), 0);
            if (retval > 0) {
                bool signal = *reinterpret_cast<bool*>(buffer);
                if (signal) {
                    start_signals[i] = true;
                    printf("클라이언트 %d가 시작 신호를 보냈습니다.\n", i);
                }
            }
            else if (retval == 0) {
                printf("클라이언트 %d 연결 종료.\n", i);
                return 1;
            }
            else {
                printf("클라이언트 %d로부터 신호 수신 오류: %d\n", i, WSAGetLastError());
                return 1;
            }
        }

        // 두 클라이언트 모두 시작 신호를 보냈는지 확인
        if (start_signals[0] && start_signals[1]) {
            printf("모든 클라이언트로부터 시작 신호를 받았습니다. 게임을 시작합니다.\n");

            bool start_btn = true;
            for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
                retval = send(client_sockets[i], reinterpret_cast<char*>(&start_btn), sizeof(start_btn), 0);
                if (retval == SOCKET_ERROR) {
                    printf("클라이언트 %d로 시작 신호 전송 실패: %d\n", i, WSAGetLastError());
                }
            }
            printf("게임 시작 신호를 보냈습니다.\n");
            break;
        }
    }

    // 쓰레드 실행
    HANDLE combined_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);
    HANDLE KEY_thread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    
    if (KEY_thread == NULL) {
        printf("쓰레드 생성 실패: %d\n", GetLastError());
        return 1;
    }

    // 쓰레드 종료 대기
    WaitForSingleObject(KEY_thread, INFINITE);
    WaitForSingleObject(combined_thread, INFINITE);
    CloseHandle(KEY_thread);
    CloseHandle(combined_thread);

    // 소켓 닫기
    for (int i = 0; i < client_count; i++) {
        shutdown(client_sockets[i], SD_BOTH);
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}