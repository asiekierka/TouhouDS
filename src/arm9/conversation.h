#ifndef CONVERSATION_H
#define CONVERSATION_H

#include "thcommon.h"

class Conversation {
	private:
		Game* game;
		SpellBook* spellbook;
		u16 buffer[256*192];
		u16 images[2][112*112];

		void DrawText(const char* name, const char* text);

	public:
		Conversation(Game* game, SpellBook* spellbook);
		virtual ~Conversation();

		bool IsFinished();

		void Image(u8 slot, const char* path);
		void Say(u8 slot, const char* name, const char* text);
		void Update();
		void Draw();
};

#endif
