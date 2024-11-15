#include "Common.h"
#include <string>
#include <chrono>
#include <thread>
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2

void Timer(SOCKET client_socket1, SOCKET client_socket2) {
    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // 시작 시간 기록
    int retval;

    while (true) {
        // 경과 시간 계산
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float 형식으로 경과 시간 저장
        inner_time = static_cast<int>(elapsed.count()); // 경과 시간을 int로 변환

        // 두 소켓에 경과 시간 전송
        retval = send(client_socket1, reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
        retval = send(client_socket2, reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);

        printf("\r경과시간: %d초", inner_time); // 서버에서 경과 시간 출력

        std::this_thread::sleep_for(std::chrono::seconds(1)); // 데이터 너무 많이 가는 문제 방지
    }//우선 테스트 클라로 시간 값 가는거 확인 완료 아마 쓰레드로 바꿔야 할거같음
}

int main(int argc, char* argv[]) {
    int retval;

    // 윈속 초기화
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // 소켓 생성
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

    // 데이터 통신에 사용할 변수
    SOCKET client_sock , client_socket1, client_socket2;
    struct sockaddr_in clientaddr1;
    int addrlen;
    char buf[BUFSIZE + 1];
    bool game_start_count = false; // 이게 true가 되면 게임 시작된 것을 알림
    bool client_sock1_connect = false; // 클라이언트 1 연결 확인
    
    bool start_btn = true; // 게임 시작값
    int client_count = 0; // 2인 플레이까지만 하도록 제한하기 위함
    std::string ip; // 클라이언트에서 주는 ip 한번만 필요
    unsigned int nMessageID = 0; // 클라이언트에서 주는 버튼값 입력 한번만 필요
    


    while (1) {
        // accept()
        // 클라이언트 접속
        if (client_count < MAX_CLIENT_COUNT) {
            addrlen = sizeof(clientaddr1);
            client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr1, &addrlen);
            if (client_sock == INVALID_SOCKET) {
                err_display("accept()");
                break;
            }
            
            // 접속한 클라이언트 정보 출력
            char addr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientaddr1.sin_addr, addr, sizeof(addr));

            printf("\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\n", addr, ntohs(clientaddr1.sin_port));

            retval = recv(client_sock, buf, BUFSIZE, 0); // 버튼 누른 거 확인
            buf[retval] = '\0';

            printf("%s\n", buf);
            
            if(client_count ==0){
                client_socket1 = client_sock;
            }
            if (client_count == 1) {
                client_socket2 = client_sock;
            }
            client_sock1_connect = true; // 클라이언트 1 연결 완료
            client_count++; // 클라이언트 1 연결 시 1 증가
            printf(" % d", client_count);
            // 두 번째 클라이언트 접속 대기
            if (client_count == 1) {
                printf("\n클라이언트 이 접속하였습니다. 클라이언트 2를 기다립니다...\n");
            }
        }

        
        
            if (client_count == 2) {
                snprintf(buf, sizeof(buf), "%d", start_btn);
                retval = send(client_socket1, buf, strlen(buf), 0);
                retval = send(client_socket2, buf, strlen(buf), 0);
                printf("클라 전송완료\n");
                
                game_start_count = true;
            }

         
        // 게임 시작 조건 (여기서 게임 로직 추가 가능)
        if (client_count == MAX_CLIENT_COUNT) {
            if (game_start_count) {
                Timer(client_socket1, client_socket2);
            }
            // 두 클라이언트 모두 접속했으므로 게임 시작 대기
            printf("\n클라이언트 2가 접속하였습니다. 게임을 시작합니다.\n");

            // recv_thread(); // 여기에 데이터 수신 처리
            // send_thread(); // 여기에 데이터 송신 처리
            break; // 예시로 게임 시작 후 서버를 종료하는 코드
        }
    }

    
    

    // 소켓 닫기
    closesocket(client_sock);
    closesocket(listen_sock);

    // 윈속 종료
    WSACleanup();
    return 0;
}
