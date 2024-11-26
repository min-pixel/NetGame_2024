#include "Common.h"


enum PacketType {
    MESH_PACKET,
    KEY_CONTROL_PACKET,
    ITEM_PACKET
};


struct PlayerMeshPacket {
    PacketType packetType = MESH_PACKET;  // 기본값 설정
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

//아이템 일단 바꾸는 것만 해놨습니다.
//교수님이 상대 블럭을 파괴하거나 그런 건 하지 말라 하셔서 하나만 받아오게 했습니다.
struct PlayerItemPacket {
    int m_nChangeBlockCount;
};
