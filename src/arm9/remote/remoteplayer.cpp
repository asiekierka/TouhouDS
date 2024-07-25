#include "remoteplayer.h"
#include "../controls.h"

void setPlayerUpdatePacket(Remote* remote, UDPPacket* packet, u32 gameFrame,
		Player* player, u8 keys)
{
	remote->InitPacket(packet, PT_playerUpdate, player->playerId);

	packet->playerUpdate.gameFrame = gameFrame;

	RemotePlayerState& rpstate = packet->playerUpdate.playerState;
	rpstate.keys = keys;
	rpstate.lives = player->GetLives();
	rpstate.shotPower = player->GetShotPower();
	rpstate.x = player->x;
	rpstate.y = player->y;
}

//-----------------------------------------------------------------------------

RemotePlayer::RemotePlayer(Game* game, u8 pid, Texture* texture,
		const char* charaId, bool gm)
:	Player(game, pid, texture, charaId), ghostMode(gm)
{
	type = SPRITE_remote;

	if (ghostMode) {
		power = 0;
	}

	state.x = x;
	state.y = y;
	state.shotPower = GetShotPower();
	state.lives = GetLives();
	state.keys = 0;
}

RemotePlayer::~RemotePlayer() {

}

bool RemotePlayer::IsInvincible() {
	return true;
}

u8 RemotePlayer::GetKeys() {
	return state.keys;
}

void RemotePlayer::Update() {
	x = state.x;
	y = state.y;
	SetLives(state.lives);
	SetShotPower(state.shotPower);

	Player::Update();

	if (ghostMode) {
		power = 0;
	}

	//Update state
	state.x = x;
	state.y = y;
	state.lives = GetLives();
	state.shotPower = GetShotPower();
	state.keys &= ~VK_bomb;
}

void RemotePlayer::Draw() {
	Player::Draw();
}

//-----------------------------------------------------------------------------
