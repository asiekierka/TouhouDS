#ifndef SELECTGAMESCREEN_H
#define SELECTGAMESCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"

class SelectGameScreen : public Screen, public ButtonListener {
	private:
		MainMenu2* menu;
		Button* backButton;
		Button* nextButton;
		TextScrollPane* scrollPane;
		Text* text;

		u16* topBuffer;
		int textPause;
		int textScroll;
		int textLines;
		int textLineHeight;
		u32 frame;

		void OnSelectedGameChanged();

	public:
		SelectGameScreen(GUI* gui, MainMenu2* menu);
		virtual ~SelectGameScreen();

		virtual void Activate();
		virtual void Deactivate();
		virtual void Cancel();
		virtual void OnButtonPressed(Button* button);
		virtual void Update(u32& keysDown, u32& keysHeld, touchPosition touch);
		virtual void DrawForeground();
		virtual void DrawTopScreen();

};

#endif
