#ifndef REMOTEPLAYER_H
#define REMOTEPLAYER_H

#include "remote.h"
#include "../player.h"

class RemotePlayer : public Player {
	private:
		bool ghostMode;

	public:
		RemotePlayerState state;

		RemotePlayer(Game* game, u8 pid, Texture* texture, const char* charaId,
				bool ghostMode=true);
		virtual ~RemotePlayer();

        virtual bool IsInvincible();
		virtual u8 GetKeys();

        virtual void Update();
        virtual void Draw();

};

void setPlayerUpdatePacket(Remote* remote, UDPPacket* packet, u32 gameFrame,
		Player* player, u8 keys);

#endif
