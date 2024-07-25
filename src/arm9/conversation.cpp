#include "conversation.h"
#include "gameinfo.h"
#include "controls.h"
#include "game.h"
#include "level.h"
#include "spells/spellbook.h"
#include "tcommon/text.h"

Conversation::Conversation(Game* game, SpellBook* spellbook) {
	this->game = game;
	this->spellbook = spellbook;

	memset(buffer, 0, 256*192*sizeof(u16));
	for (int n = 0; n < 2; n++) {
		memset(images[n], 0, 112*112*sizeof(u16));
	}
}
Conversation::~Conversation() {
    if (spellbook) delete spellbook;
}

bool Conversation::IsFinished() {
	return !spellbook || spellbook->finished;
}

void Conversation::DrawText(const char* name, const char* msg) {
	int offset = 256*120;

    char str[512];
    if (strlen(name) > 0) {
    	sprintf(str, "[%s]\n%s", name, msg);
    } else {
    	sprintf(str, "\n%s", msg);
    }

    Text* text = game->GetText();
    text->ClearBuffer();
    text->SetFontSize(12);
    text->SetMargins(0, 32, 0, 0);

    text->PrintString(str);

    //Blacken target buffer
    for (int n = offset; n < 256*192; n++) {
        buffer[n] = BIT(15);
    }

    //Draw antialiased to black
    text->BlitToScreen(buffer, 256, 192, 0, 0, 16, 120+8, 256, 64);
}

void Conversation::Image(u8 slot, const char* file) {
	if (slot >= 2) return;

	bool imageOK = false;
	if (file) {
		char path[MAXPATHLEN];
		sprintf(path, "games/%s/img/%s", game->GetSelectedGame()->GetId(), file);
		imageOK = loadImage(path, images[slot], NULL, 112, 112, GL_RGBA);
		if (imageOK && slot == 1) {
			//Mirror image
			register int temp;
			for (int y = 0; y < 112; y++) {
				int t = 112 * y;
				for (int x = 0; x < 112/2; x++) {
					temp = images[slot][t+x];
					images[slot][t+x] = images[slot][t+111-x];
					images[slot][t+111-x] = temp;
				}
			}
		}
	}
	if (!imageOK) {
		memset(images[slot], 0, 112*112*sizeof(u16));
	}

	//Clear image drawing area
	memset(buffer, 0, 256*120*sizeof(u16));

	//Draw images
	for (int n = 0; n < 2; n++) {
		int x = (n == 0 ? 0 : 256-112);
		blit2(images[n], 112, 112, buffer, 256, 192, 0, 0, x, 8, 112, 112);
	}
}

void Conversation::Say(u8 slot, const char* name, const char* text) {
	DrawText(name, text);
}

void Conversation::Update() {
	scanKeys();

	if (spellbook) {
		if (!spellbook->finished) {
			spellbook->Update(game, NULL);
		}
		if (spellbook->finished) {
		    vramSetBankB(VRAM_B_LCD);
			memset(VRAM_B+256*64, 0, 256*192*sizeof(u16));
		    vramSetBankB(VRAM_B_MAIN_BG_0x06000000);
		}
	}
}
void Conversation::Draw() {
	swiWaitForVBlank();
	vramSetBankB(VRAM_B_LCD);
    DC_FlushRange(buffer, 256*192*sizeof(u16));
    dmaCopy(buffer, VRAM_B+256*64, 256*192*sizeof(u16));
	vramSetBankB(VRAM_B_TEXTURE);

	drawQuad(game->texOverlay, 0, 0, VERTEX_SCALE(256), VERTEX_SCALE(192-16),
			Rect(0, 0, 256, 192));
}
