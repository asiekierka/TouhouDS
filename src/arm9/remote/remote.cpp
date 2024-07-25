#include "remote.h"

#include <netinet/in.h>
#include "remoteplayer.h"
#include "../as_lib9.h"
#include "../game.h"
#include "../tcommon/wifi.h"

#define SND_BUFFER_SIZE (32<<10)

#define UDP_SOCK_MAIN  0
#define UDP_SOCK_VOICE 1

Remote remote;
s32 micsendbuf[(MIC_BUFFER_SIZE>>3) + 1];
int micsendbufBytes = 0;

void micCallback(void* rawdata, int rawdataL) {
	s32* data = (s32*)rawdata;
	int dataL = (rawdataL >> 2);
	for (int n = 0; n < dataL; n++) {
		micsendbuf[n] = data[n];
	}
	micsendbuf[dataL] = data[dataL];

	micsendbufBytes = rawdataL;
}

//-----------------------------------------------------------------------------

Remote::Remote() {
	tcpcon = NULL;
	serverIP = 0;
	serverPort = 0;

	memset(udpSockets, 0, sizeof(udpSockets));
	for (int n = 0; n <= MAX_PLAYERS; n++) {
		clientTCP[n] = NULL;
		sndbuf[n] = NULL;
		sndbufOffset[n] = 0;
		sndch[n] = -1;
	}

	Stop();
}
Remote::~Remote() {
	Stop();
}

bool Remote::CreateUDPSockets(u16 port) {
	for (int n = 0; n < UDP_REMOTE_SOCKETS; n++) {
		udpSockets[n] = udpCreateSocket(port + n);
		if (udpSockets[n] < 0) {
			for (int x = 0; x < n; x++) {
				udpCloseSocket(udpSockets[x]);
			}
			return false;
		}
	}
	return true;
}

bool Remote::StartServer(u16 port) {
	Stop();

	isServer = true;
	tcpcon = new TCPConnection();
	if (tcpcon->Server(port, MAX_PLAYERS) < 0) {
		return false;
	}
	if (!CreateUDPSockets(port)) {
		delete tcpcon;
		tcpcon = NULL;
		return false;
	}

	serverIP = 0;
	serverPort = port;
	playerId = 1;

	InitVoice();
	return true;
}

bool Remote::StartClient(u32 ip, u16 port) {
	Stop();

	isServer = false;
	tcpcon = new TCPConnection();
	if (tcpcon->Connect(ip, port) < 0) {
		return false;
	}
	tcpcon->SetBlocking(true);

	if (!CreateUDPSockets(port)) {
		delete tcpcon;
		tcpcon = NULL;
		return false;
	}

	serverIP = ip;
	serverPort = port;

	char string[64];
	strcpy(string, VERSION_STRING);
	tcpcon->Send(string, 64);
	tcpcon->Receive(string, 64);
	string[63] = '\0';
	if (strcmp(string, "OK") != 0) {
		iprintf("ERROR: %s\n", string);
		delete tcpcon;
		tcpcon = NULL;
		return false;
	}

	tcpcon->Receive(&playerId, sizeof(playerId));
	tcpcon->SetBlocking(false);

	//Send UDP init packets until server ACK
	bool initOK = false;
	UDPPacket packet;
	do {
		InitPacket(&packet, PT_playerInit, playerId);
		udpSend(udpSockets[UDP_SOCK_MAIN], serverIP, serverPort, &packet, sizeof(UDPPacket));
		swiWaitForVBlank();
	} while (tcpcon->Receive(&initOK, sizeof(bool)) <= 0);

	InitVoice();
	return initOK;
}

void Remote::InitVoice() {
	for (int n = 1; n <= MAX_PLAYERS; n++) {
		if (sndch[n] >= 0) {
			AS_SoundStop(sndch[n]);
			sndch[n] = -1;
		}
		if (n == playerId) {
			continue;
		}

		if (!sndbuf[n]) {
			sndbuf[n] = new u16[SND_BUFFER_SIZE>>1];
		}
		memset(sndbuf[n], 0, SND_BUFFER_SIZE);
		sndbufOffset[n] = 0;

		SoundInfo snd = { (u8*)sndbuf[n], SND_BUFFER_SIZE,
	    	AS_PCM_8BIT, 12000, 127, 64, 1, 50, 0
	    };
		sndch[n] = AS_SoundPlay(snd);
	}

	micsendbufBytes = 0;
	startMic(micCallback, MIC_BUFFER_SIZE);
}

void Remote::Stop() {
	stopMic();

	if (tcpcon) {
		delete tcpcon;
		tcpcon = NULL;
	}
	for (int n = 0; n < UDP_REMOTE_SOCKETS; n++) {
		if (udpSockets[n] >= 0) {
			udpCloseSocket(udpSockets[n]);
			udpSockets[n] = -1;
		}
	}
	for (int n = 1; n <= MAX_PLAYERS; n++) {
		if (clientTCP[n]) delete clientTCP[n];
		clientTCP[n] = NULL;

		if (sndbuf[n]) {
			delete[] sndbuf[n];
			sndbuf[n] = NULL;
		}
		sndbufOffset[n] = 0;
		if (sndch[n] >= 0) {
			AS_SoundStop(sndch[n]);
			sndch[n] = -1;
		}
	}

	memset(playerIPs, 0, sizeof(playerIPs));
	memset(playerPorts, 0, sizeof(playerPorts));
	memset(packetNums, 0, sizeof(packetNums));

	playerId = 0;
	serverIP = 0;
	serverPort = 0;
}

void Remote::CheckIncomingConnections() {
    sockaddr_in sain;
	int sainL = sizeof(sockaddr);

	int sock = accept(tcpcon->sock, (sockaddr*)&sain, &sainL);
	if (sock < 0) return;

	u8 pid = 2;
	while (pid <= MAX_PLAYERS) {
		if (!clientTCP[pid]) break; //Free slot
		pid++;
	}
	if (pid > MAX_PLAYERS) {
		//No free player slots
		close(sock);
		return;
	}

	clientTCP[pid] = new TCPConnection(sock);
	clientTCP[pid]->SetBlocking(true);

	char string[64];
	clientTCP[pid]->Receive(string, 64);
	string[63] = '\0';
	if (strcmp(VERSION_STRING, string) != 0) {
		sprintf(string, "Wrong version, expected: %s", VERSION_STRING);
		clientTCP[pid]->Send(string, 64);

		delete clientTCP[pid];
		clientTCP[pid] = NULL;
		return;
	}

	strcpy(string, "OK");
	clientTCP[pid]->Send(string, 64);
	clientTCP[pid]->Send(&pid, sizeof(u8));
	clientTCP[pid]->SetBlocking(false);
}

bool Remote::ReceiveUDP(UDPPacket* packet, int channel) {
	if (udpSockets[channel] < 0) return false;

	sockaddr_in sain;
	int sainL = sizeof(sain);

	int read = udpReceive(udpSockets[channel], packet, sizeof(UDPPacket), &sain, &sainL);
	if (read != sizeof(UDPPacket)) {
		return false;
	}

	u8 pid = packet->playerId;
	if (pid < 1 || pid > MAX_PLAYERS) {
		log(EL_warning, __FILENAME__, "Received packet with invalid playerId (%d)", pid);
		return false;
	} else if (packet->packetNum <= packetNums[pid]) {
		log(EL_verbose, __FILENAME__, "Ignoring out-of-order packer (%d)", packet->packetNum);
		return false;
	}

	if (packet->packetType == PT_playerInit) {
		if (playerIPs[pid] == 0) {
			playerIPs[pid]   = ntohl(sain.sin_addr.s_addr);
			playerPorts[pid] = ntohs(sain.sin_port);

			bool initOK = true;
			clientTCP[pid]->Send(&initOK, sizeof(bool));

			iprintf("Adding player (%d) @ %08x:%d\n", pid, playerIPs[pid], playerPorts[pid]);
		}
	}

	return true;
}
int Remote::ReceiveUDP(void* out, int outL, int channel) {
	if (udpSockets[channel] < 0) return false;

	sockaddr_in sain;
	int sainL = sizeof(sain);

	return udpReceive(udpSockets[channel], out, outL, &sain, &sainL);
}

void Remote::SendUDP(const UDPPacket* packet, int channel) {
	SendUDP(packet, sizeof(UDPPacket), channel);
}
void Remote::SendUDP(const void* data, int dataL, int channel) {
	if (udpSockets[channel] < 0) return;

	for (int n = 1; n <= MAX_PLAYERS; n++) {
		if (playerIPs[n]) {
			udpSend(udpSockets[channel], playerIPs[n], playerPorts[n]+channel,
					data, dataL);
		}
	}
}

void Remote::CreateRemotePlayers(Game* game, Player** out, int offset,
		const char* charaId)
{
	while (offset <= MAX_PLAYERS) {
		if (playerIPs[offset]) {
			out[offset] = new RemotePlayer(game, offset, game->textures[TEX_player],
					charaId);
		}
		offset++;
	}
}

void Remote::InitPacket(UDPPacket* packet, PacketType type, u8 playerId) {
	memset(packet, 0, sizeof(packet));
	packet->packetType = type;
	packet->playerId  = playerId;
	packet->packetNum = ++packetNums[playerId];
}

void Remote::UpdateVoice() {
	VoiceUpdatePacket packet;
	const int packetSize = sizeof(VoiceUpdatePacket);

	//Send
	if (micsendbufBytes > 0) {
		packet.playerId = playerId;
		memcpy(packet.payload, micsendbuf, micsendbufBytes);
		micsendbufBytes = 0;

		remote.SendUDP(&packet, packetSize, UDP_SOCK_VOICE); //Send mic data chunk
	}

	//Receive
	while (ReceiveUDP(&packet, packetSize, UDP_SOCK_VOICE) == packetSize) {
		int pid = packet.playerId;
		if (sndch[pid] < 0) {
			continue;
		}

		int r = MIN(packetSize, SND_BUFFER_SIZE - (sndbufOffset[pid]<<1));
		memcpy(sndbuf[pid]+sndbufOffset[pid], packet.payload, r);
		DC_FlushRange(sndbuf[pid]+sndbufOffset[pid], r);

		if (r >= packetSize) {
			sndbufOffset[pid] += r >> 1;
		} else if (r < packetSize) {
			int copied = r;
			r = packetSize - copied;
			memcpy(sndbuf[pid], packet.payload+copied, r);
			DC_FlushRange(sndbuf[pid], r);
			sndbufOffset[pid] = r >> 1;
		}
	}
}

void Remote::UpdatePreGame() {
	static u32 frame = 0;
	frame++;

	UDPPacket packet;
	u32 oldPlayerIPs[MAX_PLAYERS+1];
	memcpy(oldPlayerIPs, playerIPs, (MAX_PLAYERS+1) * sizeof(u32));

	if (isServer) {
		CheckIncomingConnections();

		u32 pid = 1 + (frame % MAX_PLAYERS);
		if (clientTCP[pid] && playerIPs[pid]) { //Send updates to fully connected clients only
			PacketType type = PT_serverUpdate;
			clientTCP[pid]->Send(&type, sizeof(PacketType));
			clientTCP[pid]->Send(playerIPs+1,   MAX_PLAYERS * sizeof(u32));
			clientTCP[pid]->Send(playerPorts+1, MAX_PLAYERS * sizeof(u16));
		}
	} else {
		PacketType type;
		if (tcpcon->Receive(&type, sizeof(PacketType)) == sizeof(PacketType)) {
			if (type == PT_serverUpdate) {
				//Get other players' connection info from the server
				tcpcon->ReceiveNow(playerIPs+1,   MAX_PLAYERS * sizeof(u32));
				tcpcon->ReceiveNow(playerPorts+1, MAX_PLAYERS * sizeof(u32));

				//Server may not know its own connection info
				if (playerIPs[1] == 0) {
					playerIPs[1] = serverIP;
					playerPorts[1] = serverPort;
				}

				//Prevent sending updates to ourselves
				playerIPs[playerId] = 0;
				playerPorts[playerId] = 0;
			}
		}
	}

	//Receive
	while (ReceiveUDP(&packet, UDP_SOCK_MAIN)) {
		if (packet.packetType != PT_playerInit) {
			//iprintf("Impossible packet type for this state: %d\n", packet.packetType);
		}
	}

	bool changed = false;
	for (int n = 1; n <= MAX_PLAYERS; n++) {
		if (oldPlayerIPs[n] != playerIPs[n]) changed = true;
	}
	if (changed) {
		iprintf("%d :: %x:%d %x:%d\n", playerId, playerIPs[1], playerPorts[1], playerIPs[2], playerPorts[2]);
	}

	//Voice
	UpdateVoice();
}

void Remote::Update(Game* game, u32 frame) {
	UDPPacket packet;

	//Send PlayerUpdate
	if (game->players[0] && (frame & 1) == 0) {
		setPlayerUpdatePacket(this, &packet, frame, game->players[0],
				game->players[0]->GetKeys());
		packet.playerId = playerId; //map local player to server's ID for us
		SendUDP(&packet, UDP_SOCK_MAIN);
	}

	//Receive
	while (ReceiveUDP(&packet, UDP_SOCK_MAIN)) {
		if (packet.packetType == PT_playerUpdate) {
			RemotePlayer* player = (RemotePlayer*)game->players[packet.playerId];
			if (player) {
				memcpy(&player->state, &packet.playerUpdate.playerState, sizeof(RemotePlayerState));
				if (frame > packet.playerUpdate.gameFrame + MAX_LAG_FRAMES) {
					iprintf("WAIT:%d\n", frame - packet.playerUpdate.gameFrame);
					swiWaitForVBlank();
				}
			}
		}
	}

	//Voice
	UpdateVoice();
}
