#include "Packet.h"
#include <iostream>
char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAX_CLIENT_COUNT 2

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
            case MESH_PACKET: {
                printf("MESH 패킷 타입\n");
                break;
            }
            default:
                printf("알 수 없는 패킷 타입\n");
                continue;
            }
        }
    }
    return 0;
}




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

    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        retval = send(client_sockets[i], reinterpret_cast<char*>(&start_btn), sizeof(start_btn), 0);
        if (retval == SOCKET_ERROR) {
            printf("TCP send time error: %d\n", WSAGetLastError());
            break;
        }
    }
    printf("시작값 보냄\n");

    // 쓰레드 실행
    HANDLE KEY_thread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    if (KEY_thread == NULL) {
        printf("쓰레드 생성 실패: %d\n", GetLastError());
        return 1;
    }

    // 쓰레드 종료 대기
    WaitForSingleObject(KEY_thread, INFINITE);
    CloseHandle(KEY_thread);

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