#include "Common.h"


enum PacketType {
    MESH_PACKET,
    KEY_CONTROL_PACKET,
    ITEM_PACKET
};


struct PlayerMeshPacket {
    PacketType packetType = MESH_PACKET;  // �⺻�� ����
    float m_fxPosition;
    float m_fyPosition;
    int meshIndex;
    int topPositionRate;
    bool Win;
};

struct PlayerKeyControlPacket {
    PacketType packetType = KEY_CONTROL_PACKET;
    uint32_t nMessageID;
};

//������ �ϴ� �ٲٴ� �͸� �س����ϴ�.
//�������� ��� ���� �ı��ϰų� �׷� �� ���� ���� �ϼż� �ϳ��� �޾ƿ��� �߽��ϴ�.
struct PlayerItemPacket {
    int m_nChangeBlockCount;
};
