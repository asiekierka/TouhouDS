#ifndef MAINSCREEN_H
#define MAINSCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"

class MainScreen : public Screen, public ButtonListener {
	private:
		MainMenu2* menu;
		Button* singlePlayerButton;
		Button* practiceButton;
		Button* multiPlayerButton;
		Button* wifiButton;
		Button* scoreButton;
		Button* configButton;
		Button* backButton;

	public:
		MainScreen(GUI* gui, MainMenu2* menu);
		virtual ~MainScreen();

		virtual void OnButtonPressed(Button* button);
		virtual void DrawBackground();
		virtual void Activate();
		virtual void Deactivate();
		virtual void Cancel();

};

#endif
