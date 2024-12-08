#include "Common.h"


enum PacketType {
    GET_PLAYER_PACKET,
    MESH_PACKET,
    KEY_CONTROL_PACKET,
    ITEM_PACKET
};
#pragma pack(push, 1)
struct PlayerPacket {
    PacketType packetType;  // �⺻�� ����
    int size;
    
};

struct getPlayer {
    PacketType packetType = GET_PLAYER_PACKET;
    int count;
};

struct PlayerMeshPacket {
    PacketType packetType = MESH_PACKET;  // �⺻�� ����
    int size;
    float m_fxPosition;
    float m_fyPosition;
    int meshIndex;
    int topPositionRate;
    bool Win;
};

struct PlayerKeyControlPacket {
    PacketType packetType = KEY_CONTROL_PACKET;
    int size;
    uint32_t nMessageID;
};

//������ �ϴ� �ٲٴ� �͸� �س����ϴ�.
//�������� ��� ���� �ı��ϰų� �׷� �� ���� ���� �ϼż� �ϳ��� �޾ƿ��� �߽��ϴ�.
struct PlayerItemPacket {
    PacketType packetType = ITEM_PACKET;
    int m_nChangeBlockCount;
};
#pragma pack(pop)