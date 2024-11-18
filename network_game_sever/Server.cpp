#include "Common.h"
#include <string>
#include <chrono>
#include <thread>
#include <windows.h>  // HANDLE, CreateThread, etc.
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2
#define UDP_SERVERPORT 9500

// UDP Thread �Լ�
DWORD WINAPI UDP_thread(LPVOID param) {
    SOCKET udp_sock = (SOCKET)param;  // ���޵� UDP ������ �޾Ƽ� ���
    struct sockaddr_in udp_clientaddr;
    

    int udp_addrlen;
    char udp_buf[BUFSIZE + 1];
    int udp_retval;
    int udp_count = 0;
    int udp_client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int port1;
    // Ŭ���̾�Ʈ�� ������ ���
    while (1) {
        // ������ �ޱ�
        udp_addrlen = sizeof(udp_clientaddr);
        udp_retval = recvfrom(udp_sock, udp_buf, BUFSIZE, 0,
            (struct sockaddr*)&udp_clientaddr, &udp_addrlen);

        if (udp_retval == SOCKET_ERROR) {
            printf("UDP recvfrom error\n");
            break;
        }

        // Ŭ���̾�Ʈ �ּ� ���
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &udp_clientaddr.sin_addr, addr, sizeof(addr));
        int client_port = ntohs(udp_clientaddr.sin_port);

        // ���� ������ ���
        udp_buf[udp_retval] = '\0';  // ���� �����Ϳ� NULL ���� ���� �߰�
        printf("�޾ҽ��ϴ� udp %s   �ּ� : %s ��Ʈ: %d\n", udp_buf, addr, ntohs(udp_clientaddr.sin_port));
        
        // Ŭ���̾�Ʈ ��Ʈ�� �̹� �����ϴ��� Ȯ��

        bool isNewClient = true;
        for (int i = 0; i < udp_count; i++) {
            if (udp_client_sockets[i] == client_port) {
                isNewClient = false;
                break;
            }
        }

        // ���ο� Ŭ���̾�Ʈ�� ���, �迭�� ����
        if (isNewClient && udp_count < MAX_CLIENT_COUNT) {
            udp_client_sockets[udp_count] = client_port;
            udp_count++;  // ���ο� Ŭ���̾�Ʈ�� �߰��Ǹ� ī��Ʈ ����
            printf("�� Ŭ���̾�Ʈ ��Ʈ: %d, ������ �ֽ��ϴ�\n", client_port);
        }

        // �� Ŭ���̾�Ʈ�� ��� ����Ǿ�����, ���� �����͸� �ְ����
        if (udp_count == 2) {
            // ��Ʈ ��ȣ�� ���� �޽����� �ٸ� Ŭ���̾�Ʈ���� ����
            if (client_port == udp_client_sockets[0]) {
                udp_clientaddr.sin_port = htons(udp_client_sockets[1]);
                sendto(udp_sock, udp_buf, udp_retval, 0, (struct sockaddr*)&udp_clientaddr, sizeof(udp_clientaddr));
                printf("���½��ϴ�1 %s\n",udp_buf);
            }
            else if (client_port == udp_client_sockets[1]) {
                udp_clientaddr.sin_port = htons(udp_client_sockets[0]);
                sendto(udp_sock, udp_buf, udp_retval, 0, (struct sockaddr*)&udp_clientaddr, sizeof(udp_clientaddr));
                printf("���½��ϴ�2 %s\n", udp_buf);
            }
        }

        fflush(stdout);  // ��� ���۸� ��� �÷���

        


        
    }

    return 0;  // ������ ����
}

// Timer �Լ� (����ó�� ��� �ð� ��� �� ����)
DWORD WINAPI Timer(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // Ŭ���̾�Ʈ ���� �迭
    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // ���� �ð� ���
    int retval;

    while (true) {
        // ��� �ð� ���
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float �������� ��� �ð� ����
        inner_time = static_cast<int>(elapsed.count()); // ��� �ð��� int�� ��ȯ

        // ��� ���Ͽ� ��� �ð� ����
        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
            
        }

        printf("���� ����ð�: %d��\n", inner_time); // �������� ��� �ð� ���

        std::this_thread::sleep_for(std::chrono::seconds(1)); // ������ �ʹ� ���� ���� ���� ����
    }
    return 0;
}

int main(int argc, char* argv[]) {
    int retval;
    int udp_retval;
    WSADATA wsa;

    // ���� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
        return 1;

    // TCP ���� ����
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

    // UDP ���� ����
    SOCKET udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock == INVALID_SOCKET) err_quit("socket()");

    printf("UDP���Ͽ��Ἲ��\n");

    // UDP bind()
    struct sockaddr_in udp_serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    udp_serveraddr.sin_family = AF_INET;
    udp_serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    udp_serveraddr.sin_port = htons(UDP_SERVERPORT);
    udp_retval = bind(udp_sock, (struct sockaddr*)&udp_serveraddr, sizeof(udp_serveraddr));
    if (udp_retval == SOCKET_ERROR) err_quit("bind()");



    // Ŭ���̾�Ʈ ���� �迭
    SOCKET client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int client_count = 0;

    // Ŭ���̾�Ʈ ���� ó��
    while (client_count < MAX_CLIENT_COUNT) {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET client_socket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        

        // Ŭ���̾�Ʈ ���� �迭�� ����
        client_sockets[client_count] = client_socket;

        // ������ Ŭ���̾�Ʈ ���� ���
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("[TCP ����] Ŭ���̾�Ʈ ����: IP �ּ�=%s, ��Ʈ ��ȣ=%d\n", addr, ntohs(clientaddr.sin_port));

        client_count++; // Ŭ���̾�Ʈ ī��Ʈ ����
        

        // Ŭ���̾�Ʈ 1 ��� �޽��� ���
        if (client_count == 1) {
            printf("Ŭ���̾�Ʈ 1 ���� �Ϸ�. Ŭ���̾�Ʈ 2�� ��ٸ��ϴ�...\n");
        }
    }

    // ��� Ŭ���̾�Ʈ ���� �Ϸ�
    printf("\nŬ���̾�Ʈ 2 ���� �Ϸ�. ������ �����մϴ�.\n");

    // Timer ������ ����
    HANDLE timer_thread = CreateThread(NULL, 0, Timer, (LPVOID)client_sockets, 0, NULL);
    HANDLE udp_thread = CreateThread(NULL, 0, UDP_thread, (LPVOID)udp_sock, 0, NULL);

    // ������ ���� ���
    WaitForSingleObject(timer_thread, INFINITE);
    WaitForSingleObject(udp_thread, INFINITE);
    CloseHandle(timer_thread);
    CloseHandle(udp_thread);
    // ���� �ݱ�
    for (int i = 0; i < client_count; i++) {
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}