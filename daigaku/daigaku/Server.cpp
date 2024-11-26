#include "Packet.h"
// HANDLE, CreateThread, etc.
#define SERVERPORT 9000
#define BUFSIZE    512
#define MAX_CLIENT_COUNT 2
#define UDP_SERVERPORT 9500


DWORD WINAPI Combined_TCP_Thread(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // Ŭ���̾�Ʈ ���� �迭
    char buffer[BUFSIZE + 1];
    int retval;
    int inner_time;

    auto start = std::chrono::high_resolution_clock::now(); // ���� �ð� ���

    fd_set readfds;  // �б� ������ ������ ������ ���� //�̺κ� select GPT��� ���� �غ����ҵ� ������ �ȵ��ö� �����ߴ°Ŷ�

    while (true) {
        // ��� �ð� ���
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float �������� ��� �ð� ����
        inner_time = static_cast<int>(elapsed.count());

        // �б� ������ ������ Ȯ��
        FD_ZERO(&readfds);  // ������ ������ ���� ���� �ʱ�ȭ
        FD_SET(client_sockets[0], &readfds);  // ù ��° Ŭ���̾�Ʈ ������ �߰�
        FD_SET(client_sockets[1], &readfds);  // �� ��° Ŭ���̾�Ʈ ������ �߰�

        // select ȣ���Ͽ� �б� ������ ������ ��ٸ� (Ÿ�Ӿƿ��� 0.1�ʷ� ����)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 100000;

        retval = select(0, &readfds, NULL, NULL, &timeout);
        if (retval == SOCKET_ERROR) {
            printf("select error\n");
            break;
        }

        if (retval == 0) {
            // Ÿ�Ӿƿ� �߻� (�����Ͱ� ������ ��� ���)
            continue;
        }

        // �����͸� ������ �� �ִ� ���� Ȯ��
        for (int i = 0; i < 2; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {  // �ش� ������ �غ�Ǿ��� ���� ó��
                // ������ ����
                retval = recv(client_sockets[i], buffer, BUFSIZE, 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP recv error (Ŭ���̾�Ʈ %d)\n", i);
                    continue; // ������ �߻��� ��� �� Ŭ���̾�Ʈ�� �ǳʶٰ�, ���� Ŭ���̾�Ʈ�� �̵�
                }

                if (retval == 0) {
                    // Ŭ���̾�Ʈ�� ������ ������ ���
                    printf("Ŭ���̾�Ʈ %d�� ������ �����߽��ϴ�.\n", i);
                    continue; // ���� ����� Ŭ���̾�Ʈ�� ó������ �ʰ� �Ѿ�ϴ�.
                }

                // ���� ������ ó��
                PlayerMeshPacket* receivedPacket = (PlayerMeshPacket*)buffer;

                // ���� ������ ���
                printf("���� ������(Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                    i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

                // ��� Ŭ���̾�Ʈ�� ������ ����
                int targetClient = (i == 0) ? 1 : 0;  // ���� Ŭ���̾�Ʈ�� 0�̸� 1��
                retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP send error (Ŭ���̾�Ʈ %d -> Ŭ���̾�Ʈ %d)\n", i, targetClient);
                    continue; // ���� ������ �߻��� ���, �ش� ������ �ǳʶٰ� ��� ����
                }
               
                printf("���½��ϴ�(Ŭ���̾�Ʈ %d -> Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                    i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
            }
        }

        // ��� �ð��� 1�ʸ��� ��� Ŭ���̾�Ʈ�� ����
        for (int i = 0; i < 2; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);

            if (retval == SOCKET_ERROR) {
                printf("TCP send time error (Ŭ���̾�Ʈ %d)\n", i);
                continue; // �ð� ���� ������ �߻��ϸ� �ش� Ŭ���̾�Ʈ�� �ǳʶٰ� ��� ����
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
            //��������κ�
           

        }

        printf("�ð� ������ %d\n", inner_time);

        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // ��� ��� �� ��� ó��
    }

    return 0;  // ������ ����
}
//tcp ��Ŷ ����
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
    HANDLE timer_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);
    TcpThread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    //player key recv

    // ������ ���� ���
    WaitForSingleObject(Combined_TCP_Thread, INFINITE);
    WaitForSingleObject(ITEM_KEY_Packet, INFINITE);
    CloseHandle(TcpThread);
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