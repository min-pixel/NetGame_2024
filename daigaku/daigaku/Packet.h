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
};

struct PlayerKeyControlPacket {
    PacketType packetType = KEY_CONTROL_PACKET;
    uint32_t nMessageID;
};
struct PlayerItemPacket {
    PacketType packetType = ITEM_PACKET;
};