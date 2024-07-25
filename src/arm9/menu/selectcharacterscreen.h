#ifndef SELECTCHARACTERSCREEN_H
#define SELECTCHARACTERSCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"

class CharaButtonGrid : public Widget {
	private:
		const static int iconSize = 64 * 48;
		const static int rows = 3;
		const static int cols = 3;

		u16* icons;

	public:
		int scroll;
		int selected;

		CharaButtonGrid(Screen* parent);
		virtual ~CharaButtonGrid();

		void LoadIcons();
		void UnloadIcons();
		void Scroll(int dir);

		virtual void Update(u32& keysDown, u32& keysHeld, touchPosition touch);
		virtual void DrawBackground();
		virtual void DrawForeground();

};

class SelectCharacterScreen : public Screen, public ButtonListener {
	private:
		MainMenu2* menu;
		Button* backButton;
		Button* nextButton;
		Button* statsButton;
		CharaButtonGrid* grid;
		Button* scrollUpButton;
		Button* scrollDownButton;
		Text* text;

		u8  mode;
		u16 topBuffer[256*192];
		int textPause;
		int textScroll;
		int textLines;
		int textLineHeight;
		u32 frame;

		void SetSelectedCharacter(int c);
		void DrawStatsBackground();

	public:
		SelectCharacterScreen(GUI* gui, MainMenu2* menu);
		virtual ~SelectCharacterScreen();

		virtual void Cancel();
		virtual void Activate();
		virtual void OnButtonPressed(Button* button);
		virtual void Update(u32& keysDown, u32& keysHeld, touchPosition touch);
		virtual void DrawBackground();
		virtual void DrawForeground();
		virtual void DrawTopScreen();

};

#endif
