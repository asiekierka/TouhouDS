#ifndef SCORESCREEN_H
#define SCORESCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"
#include "../tcommon/gui/spinner.h"

class ScoreScreen : public Screen, public ButtonListener, public SpinnerListener {
	private:
		MainMenu2* menu;
		Button* backButton;
		Spinner* gameSpinner;
		Spinner* routeSpinner;
		Spinner* characterSpinner;
		TextScrollPane* scorePane;

		void OnSelectedGameChanged(GameInfo* gi);

	public:
		ScoreScreen(GUI* gui, MainMenu2* menu);
		virtual ~ScoreScreen();

		virtual void DrawBackground();
		virtual void OnSpinnerChanged(Spinner* spinner);
		virtual void OnButtonPressed(Button* button);
};

#endif
