#ifndef REMOTE_H
#define REMOTE_H

#include "../thcommon.h"
#include "../tcommon/wifi.h"
#include "remotecommon.h"

#define UDP_REMOTE_SOCKETS 2
#define MAX_LAG_FRAMES 6
#define MIC_BUFFER_SIZE (2<<10)

struct VoiceUpdatePacket {
	u8  playerId;
	u8  payload[MIC_BUFFER_SIZE/2] __attribute__ ((aligned (4)));
};

class Remote {
	private:
		u32 playerIPs[MAX_PLAYERS+1];   //Also serves as an 'isConnected' since ip(0,0,0,0) == false
		u16 playerPorts[MAX_PLAYERS+1]; //UDP ports for players
		u32 packetNums[MAX_PLAYERS+1];
		u8  playerId;					//Local playerId assigned by the server

		bool isServer;
		TCPConnection* tcpcon;
		int udpSockets[UDP_REMOTE_SOCKETS];

		u16* sndbuf[MAX_PLAYERS+1];
		s16  sndbufOffset[MAX_PLAYERS+1];
		s8   sndch[MAX_PLAYERS+1];

		//Server only
		TCPConnection* clientTCP[MAX_PLAYERS+1]; //TCP sockets of connected clients

		//Client only
		u32 serverIP;
		u16 serverPort;

		//Functions
		bool CreateUDPSockets(u16 port);
		void InitVoice();
		void UpdateVoice();

	public:
		Remote();
		~Remote();

		bool StartServer(u16 port=REMOTE_PORT);
		bool StartClient(u32 ip, u16 port=REMOTE_PORT);
		void Stop();

		void CheckIncomingConnections();
		void InitPacket(UDPPacket* packet, PacketType type, u8 playerId);
		bool ReceiveUDP(UDPPacket* packet, int channel);
		int  ReceiveUDP(void* out, int outL, int channel);
		void SendUDP(const UDPPacket* packet, int channel);
		void SendUDP(const void* data, int dataL, int channel);
		void CreateRemotePlayers(Game* game, Player** out, int offset, const char* charaId);
		void UpdatePreGame();
		void Update(Game* game, u32 frame);
};

extern Remote remote;

#include "remoteplayer.h"

#endif
