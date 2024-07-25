#ifndef SELECTROUTESCREEN_H
#define SELECTROUTESCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"

class SelectRouteScreen : public Screen, public ButtonListener {
	private:
		MainMenu2* menu;
		Button* leftButton;
		Button* rightButton;
		Button* backButton;
		Button* nextButton;
		Text* text;

		void OnSelectedRouteChanged();

	public:
		SelectRouteScreen(GUI* gui, MainMenu2* menu);
		virtual ~SelectRouteScreen();

		virtual void OnButtonPressed(Button* button);
		virtual void DrawBackground();
		virtual void DrawForeground();

};

#endif
