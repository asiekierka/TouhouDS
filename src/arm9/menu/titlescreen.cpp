#include "titlescreen.h"
#include "mainscreen.h"
#include "../tcommon/gui/gui_common.h"

#define MAX_BLINK 60

TitleScreen::TitleScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	blinking = 0;
}

TitleScreen::~TitleScreen() {
}

void TitleScreen::Activate() {
	if (!background) {
		background = new u16[256*192];
	}

	Screen::Activate();

	SetTexture(menu->titleTextureI);
	SetBackgroundImage(menu->titleI+256*192);

	blinking = 0;
}
void TitleScreen::Deactivate() {
	Screen::Deactivate();

	SetTexture(NULL);
	SetBackgroundImage(NULL);

	if (background) {
		delete[] background;
		background = NULL;
	}
}

void TitleScreen::Update(u32& down, u32& held, touchPosition touch) {
	Screen::Update(down, held, touch);

	if (blinking == 0) {
		if (down & (KEY_A|KEY_START|KEY_TOUCH)) {
			menu->soundManager.PlaySound("press_start.wav");
			blinking = 1;
		}
	} else {
		blinking++;
		if (blinking >= MAX_BLINK) {
			blinking = 0;
			gui->PushScreen(new MainScreen(gui, menu));
		}
	}
}

void TitleScreen::DrawTopScreen() {
	dmaCopy(menu->titleI, gui->sub_bg, 256*192*sizeof(u16));
}
void TitleScreen::DrawForeground() {
	Screen::DrawForeground();

	if ((blinking & 8) == 0) {
		int w = 64;
		int h = 16;

		drawQuad(texture, inttof32(128-w/2), inttof32(96-h/2),
				VERTEX_SCALE(w), VERTEX_SCALE(h), Rect(0, 224, w, h));
	}
}
