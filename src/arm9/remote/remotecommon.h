#ifndef REMOTECOMMON_H
#define REMOTECOMMON_H

#include "../thcommon.h"
#include "remotesettings.h"

#define REMOTE_PORT 28290

struct RemotePlayerState {
	s32 x, y;
	u16 shotPower;
	u8  lives;
	u8  keys;
};

enum PacketType {
	PT_playerInit,
	PT_serverUpdate,
	PT_playerUpdate
};

struct UDPPacket {
	PacketType packetType;
	u8  playerId;
	u32 packetNum;

	union {
		struct {
			u32 gameFrame;
			RemotePlayerState playerState;
		} playerUpdate;
	};
};

void initPacket(UDPPacket* packet, PacketType type, u8 playerId);

#endif
