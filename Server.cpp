#include "Packet.h"

char* SERVERIP = (char*)"127.0.0.1";
#define SERVERPORT 9000
#define BUFSIZE 4096
#define MAX_CLIENT_COUNT 2

DWORD WINAPI ITEM_KEY_Packet(LPVOID param) {
    SOCKET* client_sockets = (SOCKET*)param;
    char key_bufs[MAX_CLIENT_COUNT][BUFSIZE + 1];
    char item_bufs[MAX_CLIENT_COUNT][BUFSIZE + 1];
    int retval;
    PacketType* packetTp;

    while (1) {
        for (int i = 0; i < MAX_CLIENT_COUNT; i++) {
            retval = recv(client_sockets[i], key_bufs[i], sizeof(key_bufs[i]), 0);
            if (SOCKET_ERROR == retval) {
                printf("recv ���� %d��° ����: %d\n", i, WSAGetLastError());
                continue;
            }

            packetTp = (PacketType*)key_bufs[i];
            switch (*packetTp) {
            case KEY_CONTROL_PACKET: {
                PlayerKeyControlPacket* recvKeyPacket = (PlayerKeyControlPacket*)key_bufs[i];
                printf("Ŭ���̾�Ʈ %d���� Ű ������ ����: %d\n", i, recvKeyPacket->nMessageID);

                // Ű ������ ó�� �� ����
                retval = send(client_sockets[i], (char*)recvKeyPacket, sizeof(PlayerKeyControlPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("send ���� %d��° ����: %d\n", i, WSAGetLastError());
                }
                break;
            }
            case ITEM_PACKET: {
                PlayerItemPacket* recvItemPacket = (PlayerItemPacket*)key_bufs[i];
                printf("Ŭ���̾�Ʈ %d���� ������ ������ ����: %d\n", i, recvItemPacket->m_nChangeBlockCount);

                // ������ ������ ó�� �� ����
                retval = send(client_sockets[i], (char*)recvItemPacket, sizeof(PlayerItemPacket), 0);
                if (SOCKET_ERROR == retval) {
                    printf("send ���� %d��° ����: %d\n", i, WSAGetLastError());
                }
                break;
            }
            default:
                printf("�� �� ���� ��Ŷ Ÿ��\n");
                continue;
            }
        }
    }
    return 0;
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
    HANDLE KEY_thread = CreateThread(NULL, 0, ITEM_KEY_Packet, (LPVOID)client_sockets, 0, NULL);
    if (KEY_thread == NULL) {
        printf("������ ���� ����: %d\n", GetLastError());
        return 1;
    }

    // ������ ���� ���
    WaitForSingleObject(KEY_thread, INFINITE);
    CloseHandle(KEY_thread);

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
