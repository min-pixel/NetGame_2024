#pragma once
#include <string>
#include <cstdint> // uint32_t을 위한 헤더 추가
#include "GameObject.h" // GameObject가 정의된 헤더 파일 경로




enum PacketType {
    GET_PLAYER_PACKET,
    MESH_PACKET,
    KEY_CONTROL_PACKET
};



struct getPlayer {
    PacketType packetType = GET_PLAYER_PACKET;
    std::string ip;            
    uint32_t nMessageID;
    bool accept_server;
};

//struct PlayerMeshPacket {
//    PacketType packetType = MESH_PACKET;
//    float m_fxPosition;
//    float m_fyPosition;
//    //float m_fMovingSpeed;
//    //GameObject* m_pNextGameObjectOne;
//};

struct PlayerKeyControl {
    PacketType packetType = KEY_CONTROL_PACKET;
    uint32_t nMessageID;
};
