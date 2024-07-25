#include "selectcharacterscreen.h"
#include "thtextscrollpane.h"
#include "../bombinfo.h"
#include "../charinfo.h"
#include "../tcommon/text.h"
#include "../tcommon/gui/gui_common.h"

//-----------------------------------------------------------------------------

CharaButtonGrid::CharaButtonGrid(Screen* parent) : Widget(parent) {
	scroll = 0;
	selected = 0;
	icons = NULL;
}
CharaButtonGrid::~CharaButtonGrid() {
	if (icons) delete[] icons;
}

void CharaButtonGrid::LoadIcons() {
	if (icons) delete[] icons;
	icons = new u16[iconSize * charInfoL];

	char path[MAXPATHLEN];
	for (int n = 0; n < charInfoL; n++) {
		sprintf(path, "chara/%s/icon", charInfo[n].GetId());

		if (!loadImage(path, icons+iconSize*n, NULL, 64, 48, GL_RGBA)) {
			for (int x = 0; x < iconSize; x++) {
				icons[iconSize*n+x] = BIT(15);
			}
		}
	}

	SetBackgroundDirty();
}
void CharaButtonGrid::UnloadIcons() {
	if (icons) delete[] icons;
	icons = NULL;
}

void CharaButtonGrid::Update(u32& down, u32& held, touchPosition touch) {
	Widget::Update(down, held, touch);

	Rect bounds = GetBounds();

	int sx = selected % cols;
	int sy = selected / cols;

	int s = selected;
	do {
		if (down & KEY_UP) sy--;
		if (down & KEY_DOWN) sy++;
		if (down & KEY_LEFT) sx--;
		if (down & KEY_RIGHT) sx++;

		if (sy < 0) sy = (charInfoL-1) / cols;
		if (sy > (charInfoL-1) / cols) sy = 0;
		if (sx < 0) sx = cols-1;
		if (sx > cols-1) sx = 0;

		if (sy < scroll) {
			scroll--;
			sy += cols;
			continue;
		} else if (sy >= scroll + 3) {
			scroll++;
			sy -= cols;
			continue;
		}

		s = sy*cols+sx;
	} while (s < 0 || s >= charInfoL);
	selected = s;

	if (held & KEY_TOUCH) {
		int tx = touch.px;
		int ty = touch.py;
		if (tx >= 28 && tx < 228 && ty >= 20 && ty < 172) {
			sx = cols * (tx-28) / 200;
			sy = rows * (ty-20) / 152;
		}

		int s = sy*cols+sx;
		if (s >= 0 && s < charInfoL) {
			selected = s;
		}
	}

	while (selected < scroll*cols) {
		scroll--;
		SetBackgroundDirty();
	}
	while (selected >= scroll*cols+cols*rows) {
		scroll++;
		SetBackgroundDirty();
	}
}

void CharaButtonGrid::Scroll(int dir) {
	int sx = selected % cols;
	int sy = selected / cols;

	if (dir < 0) sy -= (sy - scroll) + 1;
	if (dir > 0) sy += (sy - scroll) + 3;

	sy = MAX(0, MIN((charInfoL-1) / cols, sy));

	int s = sy*cols+sx;
	if (s >= 0 && s < charInfoL) {
		selected = s;
	}
}

void CharaButtonGrid::DrawBackground() {
	Widget::DrawBackground();

	if (!icons || !background) return;

	Rect bounds = GetBounds();
	memset(background, 0, bounds.w*bounds.h*sizeof(u16));

	int pad = 4;
	int t = scroll*cols;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < cols; x++) {
			int dx = 8-pad + (64+pad)*x;
			int dy =   pad + (48+pad)*y;

			if (t < charInfoL) {
				blit2(icons+iconSize*t, 64, 48,
						background, bounds.w, bounds.h,
						0, 0, dx, dy, 64, 48);
			}

			t++;
		}
	}
}

void CharaButtonGrid::DrawForeground() {
	Widget::DrawForeground();

	Rect bounds = GetBounds();
	int sx = selected % cols;
	int sy = selected / cols;
	int pad = 4;
	int dx = bounds.x + 8-pad + (64+pad)*sx;
	int dy = bounds.y +   pad + (48+pad)*sy;

	int vh = VERTEX_SCALE(48);
	int vs = VERTEX_SCALE(2);

	drawQuad(texture, inttof32(dx), inttof32(dy), vs, vh, Rect(32, 80, 2, 16));
	drawQuad(texture, inttof32(dx+64), inttof32(dy), -vs, vh, Rect(61, 80, 2, 16));
}

//-----------------------------------------------------------------------------

SelectCharacterScreen::SelectCharacterScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
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

	statsButton = new Button(this);
	statsButton->SetButtonListener(this);
	statsButton->SetForegroundImage(Rect(64, 128, 64, 16));
	statsButton->SetPressedImage(Rect(64, 144, 64, 16));
	statsButton->SetBounds(0, 176, 64, 16);
	statsButton->SetActivationKeys(KEY_L);

	scrollUpButton = new Button(this);
	scrollUpButton->SetButtonListener(this);
	scrollUpButton->SetForegroundImage(Rect(0, 0, 16, 16));
	scrollUpButton->SetBounds(234, 80, 16, 16);

	scrollDownButton = new Button(this);
	scrollDownButton->SetButtonListener(this);
	scrollDownButton->SetForegroundImage(Rect(0, 16, 16, 16));
	scrollDownButton->SetBounds(234, 96, 16, 16);

	grid = new CharaButtonGrid(this);
	grid->SetBounds(24, 16, 208, 160);

	text = new Text();
	text->SetFontSize(11);
	text->SetBuffer(0, 0);

	mode = 0;
	menu->selectedCharacter = -1;
}

SelectCharacterScreen::~SelectCharacterScreen() {
	if (text) delete text;
}

void SelectCharacterScreen::Cancel() {
	grid->UnloadIcons();
	text->SetBuffer(0, 0);
	dmaCopy(menu->backgroundI, gui->sub_bg, 256*192*2);

	menu->LoadTitleImages();

	Screen::Cancel();
}

void SelectCharacterScreen::Activate() {
	menu->UnloadTitleImages();

	Screen::Activate();

	text->SetBuffer(256, 192);
	grid->LoadIcons();
	SetSelectedCharacter(0);
}

bool loadCached(const char* path, u32 version, u16* rgb, u8* a) {
	FILE* file = fopen(path, "rb");
	if (!file) return false;

	u32 cachedVersion = 0;
	fread(&cachedVersion, sizeof(u32), 1, file);
	if (cachedVersion != version) return false;

	fread(rgb, sizeof(u16), 256*192, file);
	fread(a, sizeof(u8), 256*192, file);
	fclose(file);
	return true;
}
bool saveCached(const char* path, u32 version, u16* rgb, u8* a) {
	if (!mkdirs("cache/chara/portrait")) {
		return false;
	}

	FILE* file = fopen(path, "wb");
	if (!file) return false;

	fwrite(&version, sizeof(u32), 1, file);
	fwrite(rgb, sizeof(u16), 256*192, file);
	fwrite(a, sizeof(u8), 256*192, file);
	fclose(file);
	return true;
}

void SelectCharacterScreen::SetSelectedCharacter(int c) {
    menu->selectedCharacter = c;
    if (c < 0 || c >= charInfoL || !background) {
    	return;
    }

	menu->soundManager.Update();

    u16* selectedCharI = new u16[256*192];
    u8* selectedCharAlpha = new u8[256*192];

    char cachedPath[MAXPATHLEN];
    sprintf(cachedPath, "cache/chara/portrait/%s", charInfo[c].GetId());
    if (!loadCached(cachedPath, charInfo[c].GetVersion(), selectedCharI, selectedCharAlpha)) {
        char path[MAXPATHLEN];
		sprintf(path, "chara/%s/%s", charInfo[c].GetId(), charInfo[c].GetPortrait());
		if (strrchr(path, '.')) {
			//Remove file-ext
			*strrchr(path, '.') = '\0';
		}
		loadImage(path, selectedCharI, selectedCharAlpha, 256, 192);
		menu->soundManager.Update();
		saveCached(cachedPath, charInfo[c].GetVersion(), selectedCharI, selectedCharAlpha);
    }

	menu->soundManager.Update();
    memcpy(topBuffer, menu->backgroundI, 256*192*2);
	blitAlpha(selectedCharI, selectedCharAlpha, 256, 192, topBuffer, 256, 192,
			0, 0, 0, 0, 256, 192);

	darken(topBuffer, 1, 128, 0, 128, 192);

	menu->soundManager.Update();
	DC_FlushRange(topBuffer, 256*192*2);
	dmaCopyHalfWordsAsynch(1, topBuffer, gui->sub_bg, 256*192*2);
	dmaCopyHalfWordsAsynch(2, topBuffer, background, 256*192*2);

    delete[] selectedCharI;
    delete[] selectedCharAlpha;

    textScroll = -1;
    textLines = 0;
    textLineHeight = text->GetLineHeight();
    frame = 0;

	menu->soundManager.Update();
    //DrawTopScreen();
	if (!grid->IsVisible()) {
		SetBackgroundDirty();
	}
	menu->soundManager.Update();
}

void SelectCharacterScreen::DrawTopScreen() {
	CharInfo* ci = menu->GetSelectedCharacter();
	if (!ci) return;

    u16* temp = new u16[256*192];
    memcpy(temp, topBuffer, 256*192*2);
    int pad = (192-8-32) % textLineHeight;

    text->PushState();
    text->ClearBuffer();
	text->SetFontSize(11);
    text->SetMargins(128+8, 8, 8, 8);
    text->PrintString(ci->GetName());
    text->SetMargins(128+8, 8, 8 + 24, 8 + pad);
    text->SetPen(text->GetPenX(), text->GetPenY() - MAX(0, textScroll));
    textLines = text->PrintString(ci->GetDescription());
    text->BlitToScreen(temp, 256, 192, 128, 9, 128, 8, 128, 192 -8-8);

    DC_FlushRange(temp, 256*192*2);
    dmaCopy(temp, gui->sub_bg, 256*192*2);

    text->PopState();
    delete[] temp;
}

void SelectCharacterScreen::Update(u32& down, u32& held, touchPosition touch) {
	bool oldGridVisibility = grid->IsVisible();
	grid->SetVisible(mode == 0);
	if (grid->IsVisible() != oldGridVisibility) {
		SetBackgroundDirty();
	}

	if (!grid->IsVisible()) {
		if (down & KEY_UP) grid->selected--;
		if (down & KEY_DOWN) grid->selected++;

		if (grid->selected < 0) grid->selected += charInfoL;
		if (grid->selected >= charInfoL) grid->selected -= charInfoL;
	}

	if (menu->selectedCharacter != grid->selected) {
		menu->selectedCharacter = -1;
		if (grid->selected >= 0 && grid->selected < charInfoL) {
			SetSelectedCharacter(grid->selected);
		}
	}

    if (textScroll < 0) {
        textScroll = 0;
        textPause = 40;
    }

    if (textPause > 0) {
        textPause--;
    }
    if (textPause <= 0) {
        if (textPause < 0) {
            textPause++;
            if (textPause == 0) {
                textScroll = -1;
                DrawTopScreen();
            }
        } else {
            int pad = (192-8-32) % textLineHeight;
            if (textScroll < textLines * textLineHeight - (192-8-32-pad)) {
                if (frame & 31) {
                    textScroll++;
                    DrawTopScreen();
                }
            } else if (textPause == 0) {
                textPause = -40;
            }
        }
    }
	frame++;

	Screen::Update(down, held, touch);
}
void SelectCharacterScreen::DrawBackground() {
	Screen::DrawBackground();

	if (!grid->IsVisible()) {
		DrawStatsBackground();
	}
}

void SelectCharacterScreen::DrawStatsBackground() {
    memcpy(background, menu->backgroundI + 256*192, 256*192*2);

    CharInfo* ci = menu->GetSelectedCharacter();
    if (ci) {
		int lineHeight = text->GetLineHeight();
		text->SetMargins((256-192)/2, (256-192)/2, (128-lineHeight*8)/2 + 32, (128-lineHeight*8)/2 + 32);
		text->SetPen(text->GetMarginLeft(), text->GetMarginTop() + lineHeight - 2);
		int valueX = 128+16;
		int valueY = text->GetPenY()-lineHeight;

		//Darken text area
		int yMin = MAX(0, text->GetMarginTop()-8);
		int yMax = 192-MAX(0, text->GetMarginBottom()-8);
		int xMin = MAX(0, text->GetMarginLeft()-8);
		int xMax = 256-MAX(0, text->GetMarginRight()-8);
		darken(background, 1, xMin, yMin, xMax-xMin, yMax-yMin);

		//Print
	    text->PushState();
	    text->SetFontSize(12);

	    char str[32];
		text->ClearBuffer();
		char bomb1[64];
		sprintf(bomb1, "%s", bombFromString(ci->GetBomb()) ? bombFromString(ci->GetBomb())->GetName() : "Default");
		char bomb2[64];
		sprintf(bomb2, "%s", bombFromString(ci->GetBombFocus()) ? bombFromString(ci->GetBombFocus())->GetName() : "Default");

		text->PrintString("Power");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.1f", f32tofloat(ci->GetShotPowerMul()));
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Speed");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.1f", f32tofloat(ci->GetSpeed()));
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Speed (Focus)");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.1f", f32tofloat(ci->GetSpeedFocus()));
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Bombs");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.d", ci->GetBombs());
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Graze Distance");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.1f", f32tofloat(ci->GetGrazeRange()));
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Hit Distance");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%.1f", f32tofloat(ci->GetHitRadius()));
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Primary Bomb");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%15s", bomb1);
		text->PrintString(str);
		text->PrintNewline();
		text->PrintString("Secondary Bomb");
		text->SetPen(valueX, valueY+=lineHeight);
		sprintf(str, "%15s", bomb2);
		text->PrintString(str);

		text->BlitToScreen(background, 256, 192, 0, 0, 0, 0, 256, 192);
		text->PopState();
    }

   	DC_FlushRange(background, 256*192*2);
    dmaCopy(background, gui->main_bg, 256*192*2);
}
void SelectCharacterScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen(GT_fade);
	} else if (button == nextButton) {
		menu->Quit();
	} else if (button == statsButton) {
		mode = (mode != 0 ? 0 : 1);
		statsButton->SetForegroundImage(Rect(64, (mode == 1 ? 144 : 128), 64, 16));
	} else if (button == scrollUpButton) {
		grid->Scroll(-1);
	} else if (button == scrollDownButton) {
		grid->Scroll(1);
	}
}

void SelectCharacterScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(0), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));
	drawQuad(texture, inttof32(192), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));

	drawQuad(texture, inttof32(64), inttof32(176), VERTEX_SCALE(128), VERTEX_SCALE(16),
			Rect(128, 144, 128, 16));
}
