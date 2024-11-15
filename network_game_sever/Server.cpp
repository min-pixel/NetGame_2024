#include "Common.h"
#include <string>
#include <chrono>
#include <thread>
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2

void Timer(SOCKET client_socket1, SOCKET client_socket2) {
    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // ���� �ð� ���
    int retval;

    while (true) {
        // ��� �ð� ���
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float �������� ��� �ð� ����
        inner_time = static_cast<int>(elapsed.count()); // ��� �ð��� int�� ��ȯ

        // �� ���Ͽ� ��� �ð� ����
        retval = send(client_socket1, reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
        retval = send(client_socket2, reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);

        printf("\r����ð�: %d��", inner_time); // �������� ��� �ð� ���

        std::this_thread::sleep_for(std::chrono::seconds(1)); // ������ �ʹ� ���� ���� ���� ����
    }//�켱 �׽�Ʈ Ŭ��� �ð� �� ���°� Ȯ�� �Ϸ� �Ƹ� ������� �ٲ�� �ҰŰ���
}

int main(int argc, char* argv[]) {
    int retval;

    // ���� �ʱ�ȭ
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // ���� ����
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

    // ������ ��ſ� ����� ����
    SOCKET client_sock , client_socket1, client_socket2;
    struct sockaddr_in clientaddr1;
    int addrlen;
    char buf[BUFSIZE + 1];
    bool game_start_count = false; // �̰� true�� �Ǹ� ���� ���۵� ���� �˸�
    bool client_sock1_connect = false; // Ŭ���̾�Ʈ 1 ���� Ȯ��
    
    bool start_btn = true; // ���� ���۰�
    int client_count = 0; // 2�� �÷��̱����� �ϵ��� �����ϱ� ����
    std::string ip; // Ŭ���̾�Ʈ���� �ִ� ip �ѹ��� �ʿ�
    unsigned int nMessageID = 0; // Ŭ���̾�Ʈ���� �ִ� ��ư�� �Է� �ѹ��� �ʿ�
    


    while (1) {
        // accept()
        // Ŭ���̾�Ʈ ����
        if (client_count < MAX_CLIENT_COUNT) {
            addrlen = sizeof(clientaddr1);
            client_sock = accept(listen_sock, (struct sockaddr*)&clientaddr1, &addrlen);
            if (client_sock == INVALID_SOCKET) {
                err_display("accept()");
                break;
            }
            
            // ������ Ŭ���̾�Ʈ ���� ���
            char addr[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &clientaddr1.sin_addr, addr, sizeof(addr));

            printf("\n[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr1.sin_port));

            retval = recv(client_sock, buf, BUFSIZE, 0); // ��ư ���� �� Ȯ��
            buf[retval] = '\0';

            printf("%s\n", buf);
            
            if(client_count ==0){
                client_socket1 = client_sock;
            }
            if (client_count == 1) {
                client_socket2 = client_sock;
            }
            client_sock1_connect = true; // Ŭ���̾�Ʈ 1 ���� �Ϸ�
            client_count++; // Ŭ���̾�Ʈ 1 ���� �� 1 ����
            printf(" % d", client_count);
            // �� ��° Ŭ���̾�Ʈ ���� ���
            if (client_count == 1) {
                printf("\nŬ���̾�Ʈ �� �����Ͽ����ϴ�. Ŭ���̾�Ʈ 2�� ��ٸ��ϴ�...\n");
            }
        }

        
        
            if (client_count == 2) {
                snprintf(buf, sizeof(buf), "%d", start_btn);
                retval = send(client_socket1, buf, strlen(buf), 0);
                retval = send(client_socket2, buf, strlen(buf), 0);
                printf("Ŭ�� ���ۿϷ�\n");
                
                game_start_count = true;
            }

         
        // ���� ���� ���� (���⼭ ���� ���� �߰� ����)
        if (client_count == MAX_CLIENT_COUNT) {
            if (game_start_count) {
                Timer(client_socket1, client_socket2);
            }
            // �� Ŭ���̾�Ʈ ��� ���������Ƿ� ���� ���� ���
            printf("\nŬ���̾�Ʈ 2�� �����Ͽ����ϴ�. ������ �����մϴ�.\n");

            // recv_thread(); // ���⿡ ������ ���� ó��
            // send_thread(); // ���⿡ ������ �۽� ó��
            break; // ���÷� ���� ���� �� ������ �����ϴ� �ڵ�
        }
    }

    
    

    // ���� �ݱ�
    closesocket(client_sock);
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}
