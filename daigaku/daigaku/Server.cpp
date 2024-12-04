#include "Packet.h"
#include <iostream>
char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 512
#define MAX_CLIENT_COUNT 2


DWORD WINAPI Combined_TCP_Thread(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;  // Ŭ���̾�Ʈ ���� �迭
    char buffer[BUFSIZE + 1];
    int retval;
    int inner_time = 0;
    int SavePosition[2]{};
    auto start = std::chrono::high_resolution_clock::now(); // ���� �ð� ���
    const char* win_game = "�̰���ϴ�";
    const char* lose_game = "�����ϴ�";
    fd_set readfds;  // �б� ������ ������ ������ ����

    while (true) {
        // ��� �ð� ���
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - start; // float �������� ��� �ð� ����
        inner_time = static_cast<int>(elapsed.count());

        for (int i = 0; i < 2; i++) {
            retval = send(client_sockets[i], reinterpret_cast<char*>(&inner_time), sizeof(inner_time), 0);
            if (retval == SOCKET_ERROR) {
                printf("TCP send time error (Ŭ���̾�Ʈ %d)\n", i);
                continue; // �ð� ���� ������ �߻��ϸ� �ش� Ŭ���̾�Ʈ�� �ǳʶٰ� ��� ����
            }
        }
        printf("�ð� ������ %d\n", inner_time);

        // �б� ������ ������ Ȯ��
        FD_ZERO(&readfds);  // ������ ������ ���� ���� �ʱ�ȭ
        FD_SET(client_sockets[0], &readfds);  // ù ��° Ŭ���̾�Ʈ ������ �߰�
        FD_SET(client_sockets[1], &readfds);  // �� ��° Ŭ���̾�Ʈ ������ �߰�

        // select ȣ���Ͽ� �б� ������ ������ ��ٸ� (Ÿ�Ӿƿ��� 0.1�ʷ� ����)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;  // 0.2�� Ÿ�Ӿƿ�

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

                // Position �����͸� ����
                //SavePosition[i] = receivedPacket->topPositionRate;

                // ���� ������ ���
                printf("���� ������(Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                    i, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);

                // ��� Ŭ���̾�Ʈ�� ������ ����
                int targetClient = (i == 0) ? 1 : 0;  // ���� Ŭ���̾�Ʈ�� 0�̸� 1��, 1�̸� 0���� ����
                retval = send(client_sockets[targetClient], (char*)receivedPacket, sizeof(PlayerMeshPacket), 0);
                if (retval == SOCKET_ERROR) {
                    printf("TCP send error (Ŭ���̾�Ʈ %d -> Ŭ���̾�Ʈ %d)\n", i, targetClient);

                }

                printf("���½��ϴ�(Ŭ���̾�Ʈ %d -> Ŭ���̾�Ʈ %d): Position(%f, %f), Mesh Index:%d\n",
                    i, targetClient, receivedPacket->m_fxPosition, receivedPacket->m_fyPosition, receivedPacket->meshIndex);
            }
        }


        if (inner_time == 100)
        {
            //�ϴ� �̺κ� Ŭ�󿡼� �ִ� ���̰� �޾ƿ;� ���� ����

            for (int i = 0; i < 2; i++)
            {
                PlayerMeshPacket meshHeight;
                if (SavePosition[0] < SavePosition[1])
                {
                    //meshHeight.Win = (i == 1);//�̺κ� ������ �غ����ҵ� �켱 Ŭ��2�� bool�� ������  ���ڿ��� �¸� �й� �����ָ� �װ� ����ϰ�
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

                
                //���� ���� �����ϰ� ���� �Ǿ��ٶ� �� ������ ���ٵ�
                if (retval == SOCKET_ERROR)
                {
                    printf("sendERROR %d\n", i);
                }
            }
            break;
            //��������κ�


        }
        // �̺κ� Ŭ��� �������
        std::this_thread::sleep_for(std::chrono::milliseconds(200));


    }

    // ���� ���� ó��
    for (int i = 0; i < 2; i++) {
        closesocket(client_sockets[i]);
    }

    return 0;  // ������ ����
}

DWORD WINAPI ITEM_KEY_Packet(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;

    char recv_bufs[MAX_CLIENT_COUNT][BUFSIZE];
    int retval;
    fd_set readfds;
    while (1) {

        // �б� ������ ������ Ȯ��
        FD_ZERO(&readfds);  // ������ ������ ���� ���� �ʱ�ȭ
        FD_SET(client_sockets[0], &readfds);  // ù ��° Ŭ���̾�Ʈ ������ �߰�
        FD_SET(client_sockets[1], &readfds);  // �� ��° Ŭ���̾�Ʈ ������ �߰�

        // select ȣ���Ͽ� �б� ������ ������ ��ٸ� (Ÿ�Ӿƿ��� 0.1�ʷ� ����)
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = 200000;  // 0.2�� Ÿ�Ӿƿ�

        retval = select(0, &readfds, NULL, NULL, &timeout);
        if (retval == SOCKET_ERROR) {
            printf("select error\n");
            break;
        }

        if (retval == 0) {
            // Ÿ�Ӿƿ� �߻� (�����Ͱ� ������ ��� ���)
            continue;
        }





        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            if (FD_ISSET(client_sockets[i], &readfds)) {

                memset(recv_bufs[i], 0, BUFSIZE);
                retval = recv(client_sockets[i], recv_bufs[i], sizeof(PlayerPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("recv ���� %d��° ����: %d\n", i, WSAGetLastError());
                    continue;
                }

                PlayerPacket* pk = (PlayerPacket*)recv_bufs[i];
                retval = recv(client_sockets[i], recv_bufs[i] + sizeof(PlayerPacket), pk->size - sizeof(PlayerPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("recv ���� %d��° ����: %d\n", i, WSAGetLastError());
                    continue;
                }

                switch (pk->packetType) {
                case KEY_CONTROL_PACKET: {
                    PlayerKeyControlPacket* recvKeyPacket = (PlayerKeyControlPacket*)recv_bufs[i];
                    printf("Ŭ���̾�Ʈ %d���� Ű ������ ����: %d\n", i, recvKeyPacket->nMessageID);

                    // Ű ������ ó�� �� ����
                    int targetClient = (i == 0) ? 1 : 0;  // ���� Ŭ���̾�Ʈ�� 0�̸� 1��, 1�̸� 0���� ����
                    retval = send(client_sockets[targetClient], (char*)recvKeyPacket, sizeof(PlayerKeyControlPacket), 0);
                    printf("Ű ������\n");
                    if (SOCKET_ERROR == retval) {
                        printf("send ���� %d��° ����: %d\n", i, WSAGetLastError());
                    }
                    break;
                }
                case ITEM_PACKET: {
                    PlayerItemPacket* recvItemPacket = (PlayerItemPacket*)recv_bufs[i];
                    printf("Ŭ���̾�Ʈ %d���� ������ ������ ����: %d\n", i, recvItemPacket->m_nChangeBlockCount);

                    // ������ ������ ó�� �� ����
                    int targetClient = (i == 0) ? 1 : 0;
                    retval = send(client_sockets[targetClient], (char*)recvItemPacket, sizeof(PlayerItemPacket), 0);
                    printf("Ű ������\n");
                    if (SOCKET_ERROR == retval) {
                        printf("send ���� %d��° ����: %d\n", i, WSAGetLastError());
                    }
                    break;
                }

                }
            }
        }
        return 0;
    }

}


int main(int argc, char* argv[]) {
    int retval;
    WSADATA wsa;

    // ���� �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup ����: %d\n", WSAGetLastError());
        return 1;
    }

    // TCP ���� ����
    SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == INVALID_SOCKET) {
        printf("���� ���� ����: %d\n", WSAGetLastError());
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
        printf("bind() ����: %d\n", WSAGetLastError());
        return 1;
    }

    bool start_btn = true;

    // listen()
    retval = listen(listen_sock, SOMAXCONN);
    if (retval == SOCKET_ERROR) {
        printf("listen() ����: %d\n", WSAGetLastError());
        return 1;
    }

    // Ŭ���̾�Ʈ ���� �迭
    SOCKET client_sockets[MAX_CLIENT_COUNT] = { 0 };
    int client_count = 0;

    // Ŭ���̾�Ʈ ���� ó��
    while (client_count < MAX_CLIENT_COUNT) {
        struct sockaddr_in clientaddr;
        int addrlen = sizeof(clientaddr);

        SOCKET client_socket = accept(listen_sock, (struct sockaddr*)&clientaddr, &addrlen);
        if (client_socket == INVALID_SOCKET) {
            printf("accept() ����: %d\n", WSAGetLastError());
            continue;
        }

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

    for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
        retval = send(client_sockets[i], reinterpret_cast<char*>(&start_btn), sizeof(start_btn), 0);
        if (retval == SOCKET_ERROR) {
            printf("TCP send time error: %d\n", WSAGetLastError());
            break;
        }
    }
    printf("���۰� ����\n");

    // ������ ����
    HANDLE Com_thread = CreateThread(NULL, 0, Combined_TCP_Thread, (LPVOID)client_sockets, 0, NULL);

    HANDLE KEY_thread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    if (KEY_thread == NULL) {
        printf("������ ���� ����: %d\n", GetLastError());
        return 1;
    }

    // ������ ���� ���
    WaitForSingleObject(KEY_thread, INFINITE);
    WaitForSingleObject(Com_thread, INFINITE);
    CloseHandle(KEY_thread);
    CloseHandle(Com_thread);

    // ���� �ݱ�
    for (int i = 0; i < client_count; i++) {
        shutdown(client_sockets[i], SD_BOTH);
        closesocket(client_sockets[i]);
    }
    closesocket(listen_sock);

    // ���� ����
    WSACleanup();
    return 0;
}