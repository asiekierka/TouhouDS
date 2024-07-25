#ifndef PRACTICESCREEN_H
#define PRACTICESCREEN_H

#include "mainmenu2.h"
#include "../remote/remotecommon.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"
#include "../tcommon/gui/spinner.h"

class THTextScrollPane;

class PracticeScreen : public Screen, public ButtonListener, public SpinnerListener {
	private:
		RemoteSettings remoteSettings;

		MainMenu2* menu;
		Button* backButton;
		Button* nextButton;
		Spinner* gameSpinner;
		Spinner* routeSpinner;
		THTextScrollPane* stagePane;

		void OnSelectedGameChanged(GameInfo* gi);
		void OnSelectedRouteChanged(GameInfo* gi, int route);

	public:
		PracticeScreen(GUI* gui, MainMenu2* menu);
		virtual ~PracticeScreen();

		virtual void Deactivate();
		virtual void Cancel();
		virtual void OnSpinnerChanged(Spinner* spinner);
		virtual void OnButtonPressed(Button* button);
		virtual void DrawBackground();
		virtual void DrawForeground();
};

#endif
