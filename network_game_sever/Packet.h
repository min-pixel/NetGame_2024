#include "Common.h"


enum PacketType {
    MESH_PACKET,
    KEY_CONTROL_PACKET
};


struct PlayerMeshPacket {
    PacketType packetType = MESH_PACKET;  
    float m_fxPosition;
    float m_fyPosition;
    int meshIndex;
};

struct PlayerKeyControlPacket {
    PacketType packetType = KEY_CONTROL_PACKET;
    uint32_t nMessageID;
};