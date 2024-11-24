#include "Packet.h"


char* SERVERIP = (char*)"127.0.0.2";
#define SERVERPORT 9000
#define BUFSIZE 512
#define WM_LBUTTONDOWN 0x0201  // ���� �䱸 �޽��� ID ��
#define MAX_CLIENT_COUNT 2



DWORD WINAPI Combined_TCP_Thread(LPVOID param) {//TCP �κ� ���� Ű���� ���� Ű�� �����ؾ����� �ָ���
    SOCKET* client_sockets = (SOCKET*)param;  // Ŭ���̾�Ʈ ���� �迭
    char buffer[BUFSIZE + 1];
    int retval;

    int inner_time;
    auto start = std::chrono::high_resolution_clock::now(); // ���� �ð� ���

    while (true) {

        // ��� �ð� ���
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float �������� ��� �ð� ����
         // ��� �ð��� int�� ��ȯ
        inner_time = static_cast<int>(elapsed.count());
        // �����͸� ���� ��, ��� ���Ͽ��� �Դ��� Ȯ��
        for (int i = 0; i < 2; i++) {
            // ������ ����
            retval = recv(client_sockets[i], buffer, BUFSIZE, 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP recv error\n");
                break;
            }

            PlayerMeshPacket* receivedPacket = (PlayerMeshPacket*)buffer;

            // ���� ������ ���
            printf("���� ������(Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

            int targetClient = (i == 0) ? 1 : 0;  // ���� ������ 0�̸� 1������, 1�̸� 0������
            retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP send error\n");
                break;
            }

            printf("���½��ϴ�(Ŭ���̾�Ʈ %d -> Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
        }
        
        // ��� �ð��� 1�ʸ��� ��� Ŭ���̾�Ʈ�� ���� (����)
        for (int i = 0; i < 2; i++) {
              // ���÷� ������ �ð��� (��� �ð� ��)
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
           
            if (retval == SOCKET_ERROR) {
                printf("TCP send time error\n");
                break;
            }
        }
        printf("�ð� ������ %d", inner_time);
        
        
    }

    return 0;  // ������ ����
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
    bool start_btn = true;
    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) err_quit("listen()");







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

    for (int i = 0; i < 2; i++) {
        
        retval = send(client_sockets[i], reinterpret_cast<char*>(&start_btn), sizeof(start_btn), 0);

        if (retval == SOCKET_ERROR) {
            printf("TCP send time error\n");
            break;
        }
    }
    printf("���۰� ����\n");
    // Timer ������ ����
    HANDLE timer_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);


    // ������ ���� ���
    WaitForSingleObject(timer_thread, INFINITE);

    CloseHandle(timer_thread);

    // ���� �ݱ�
    for (int i = 0; i < client_count; i++) {
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}