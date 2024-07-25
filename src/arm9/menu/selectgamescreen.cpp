#include "selectgamescreen.h"
#include "selectroutescreen.h"
#include "thtextscrollpane.h"
#include "../gameinfo.h"
#include "../tcommon/text.h"
#include "../tcommon/gui/gui_common.h"

SelectGameScreen::SelectGameScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);

	nextButton = new Button(this);
	nextButton->SetButtonListener(this);
	nextButton->SetForegroundImage(Rect(160, 80, 48, 16));
	nextButton->SetBounds(208, 0, 48, 16);
	nextButton->SetActivationKeys(KEY_A|KEY_START);

	scrollPane = new THTextScrollPane(this, defaultFontCache);
	scrollPane->SetItemBg(menu->textureI+(192<<8), 256, 32, menu->textureI+(192<<8), 256, 32);
	scrollPane->SetTextWrapping(false);
	scrollPane->SetTextBufferSize(4096);
	scrollPane->SetItemHeight(32);
	scrollPane->SetDefaultMargins(40, 8, 0, 0);
	scrollPane->SetFontSize(12, false);
	scrollPane->SetBounds(0, 16, 256, 160);

	text = new Text();

	menu->selectedGame = 0;
	topBuffer = NULL;
}

SelectGameScreen::~SelectGameScreen() {
	if (topBuffer) {
		delete[] topBuffer;
		topBuffer = NULL;
	}
	delete text;
}

void SelectGameScreen::Activate() {
	Screen::Activate();

	int selected = 0;
	scrollPane->RemoveAllItems();
	for (int n = 0; n < gameInfoL; n++) {
		if (gameInfo[n]) {
			if (gameInfo[n] == menu->GetSelectedGame()) {
				selected = n;
			}

		    char versionStr[16];
		    gameInfo[n]->GetVersionString(versionStr);

		    char temp[256];
		    sprintf(temp, "%s\t%s", gameInfo[n]->GetName(), versionStr);

			scrollPane->AppendText(temp);
		}
	}
	scrollPane->SetScroll(0);
	scrollPane->SetSelectedIndex(selected);

	text->SetBuffer(256, 192);
	if (!topBuffer) topBuffer = new u16[256*192];
	OnSelectedGameChanged();
}
void SelectGameScreen::Cancel() {
	Screen::Cancel();

	if (topBuffer) {
		delete[] topBuffer;
		topBuffer = NULL;
	}
	text->SetBuffer(0, 0);
	dmaCopy(menu->titleI, gui->sub_bg, 256*192*2);
}
void SelectGameScreen::Deactivate() {
	Screen::Deactivate();

	if (topBuffer) {
		delete[] topBuffer;
		topBuffer = NULL;
	}
	text->SetBuffer(0, 0);
	dmaCopy(menu->backgroundI, gui->sub_bg, 256*192*2);
}

void SelectGameScreen::Update(u32& down, u32& held, touchPosition touch) {
	if (menu->selectedGame != scrollPane->GetSelectedIndex()) {
		menu->selectedGame = -1;
		if (scrollPane->GetSelectedIndex() >= 0 && scrollPane->GetSelectedIndex() < gameInfoL) {
			menu->selectedGame = scrollPane->GetSelectedIndex();
		}
		OnSelectedGameChanged();
	}

    if (textScroll < 0) {
        textScroll = 0;
        textPause = 40;
    }

    DrawTopScreen();

    if (textPause > 0) {
        textPause--;
    }
    if (textPause <= 0) {
        if (textPause < 0) {
            textPause++;
            if (textPause == 0) {
                textScroll = -1;
            }
        } else {
            int pad = (192-8-32) % textLineHeight;
            if (textScroll < textLines * textLineHeight - (192-8-32-pad)) {
                if (frame & 31) {
                    textScroll++;
                }
            } else if (textPause == 0) {
                textPause = -40;
            }
        }
    }
	frame++;

	Screen::Update(down, held, touch);
}

void SelectGameScreen::DrawTopScreen() {
	GameInfo* gi = menu->GetSelectedGame();
	if (!gi) return;

	bool hasNonSpaceChars = false;
	for (const char* s = gi->GetDesc(); *s != '\0'; s++) {
		if (!isspace(*s)) {
			hasNonSpaceChars = true;
			break;
		}
	}
	if (!hasNonSpaceChars) {
		return;
	}

    u16* temp = new u16[256*192];
    memcpy(temp, topBuffer, 256*192*2);
    darken(temp, 1, 128, 0, 128, 192);

    int pad = (192-8-32) % textLineHeight;

    text->PushState();
    text->ClearBuffer();
	text->SetFontSize(10);
    text->SetMargins(128+8, 8, 8, 8);
    text->PrintString(gi->GetName());
    text->PrintNewline();
    text->SetMargins(128+8, 8, 8 + 24, 8 + pad);
    text->SetPen(text->GetPenX(), text->GetPenY() - MAX(0, textScroll));
    textLines = text->PrintString(gi->GetDesc());
    text->BlitToScreen(temp, 256, 192, 128, 9, 128, 8, 128, 192 -8-8);

    DC_FlushRange(temp, 256*192*2);
    dmaCopy(temp, gui->sub_bg, 256*192*2);

    text->PopState();
    delete[] temp;
}

void SelectGameScreen::OnSelectedGameChanged() {
	GameInfo* gi = menu->GetSelectedGame();

    memcpy(topBuffer, menu->backgroundI, 256*192*2);

	if (gi) {
	    u16* pixels = new u16[256 * 192];
	    u8* alpha = new u8[256 * 192];
	    char path[PATH_MAX];
	    sprintf(path, "games/%s/%s", gi->GetId(), gi->GetBanner());
		if (!loadImage(path, pixels, alpha, 256, 192)) {
			loadImage("img/default_game_banner", pixels, alpha, 256, 192, GL_RGB8_A5);
		}
		blitAlpha(pixels, alpha, 256, 192, topBuffer, 256, 192, 0, 0, 0, 0, 256, 192);
		delete[] pixels;
		delete[] alpha;
	}

    DC_FlushRange(topBuffer, 256*192*2);
    dmaCopy(topBuffer, gui->sub_bg, 256*192*2);

    textScroll = -1;
    textLines = 0;
    textLineHeight = text->GetLineHeight();
    frame = 0;

    menu->selectedRoute = 0;
}
void SelectGameScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen(GT_fade);
	} else if (button == nextButton) {
		gui->PushScreen(new SelectRouteScreen(gui, menu), GT_fade);
	}
}

void SelectGameScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(0), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));
	drawQuad(texture, inttof32(192), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));

	drawQuad(texture, inttof32(64), inttof32(176), VERTEX_SCALE(128), VERTEX_SCALE(16),
			Rect(128, 160, 128, 16));
}
