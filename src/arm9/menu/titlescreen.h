#ifndef TITLESCREEN_H
#define TITLESCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"

class TitleScreen : public Screen {
	private:
		MainMenu2* menu;
		u32 blinking;

	public:
		TitleScreen(GUI* gui, MainMenu2* menu);
		virtual ~TitleScreen();

		virtual void Activate();
		virtual void Deactivate();
		virtual void Update(u32& down, u32& held, touchPosition touch);
		virtual void DrawTopScreen();
		virtual void DrawForeground();
};

#endif
