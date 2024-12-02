#pragma once
#include <string>
#include <cstdint> // uint32_t을 위한 헤더 추가
#include "GameObject.h" // GameObject가 정의된 헤더 파일 경로




enum PacketType {
    GET_PLAYER_PACKET,
    MESH_PACKET,
    KEY_CONTROL_PACKET,
    ITEM_PACKET
};



struct getPlayer {
    PacketType packetType = GET_PLAYER_PACKET;
    char ip[16];
    uint32_t nMessageID;
    bool accept_server;
};

#pragma pack(push, 1)
struct PlayerMeshPacket {
    PacketType packetType = MESH_PACKET;
    int size;
    float m_fxPosition;
    float m_fyPosition;
    //float m_fRotation;
    int meshIndex;  // 블록의 메쉬 타입 (0~7 범위의 인덱스)
    int topPositionRate;
    bool Win;
    //float m_fMovingSpeed;
    /*GameObject* m_pNextGameObjectOne;
    int blockType;*/
};

struct PlayerKeyControl {
    PacketType packetType = KEY_CONTROL_PACKET;
    int size;
    uint32_t nMessageID;
};

struct PlayerItemPacket {
    PacketType packetType = ITEM_PACKET;
    int m_nChangeBlockCount;
};
#pragma pack(pop)