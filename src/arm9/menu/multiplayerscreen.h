#ifndef MULTIPLAYERSCREEN_H
#define MULTIPLAYERSCREEN_H

#include "mainmenu2.h"
#include "../remote/remotecommon.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"
#include "../tcommon/gui/spinner.h"

class MultiPlayerScreen : public Screen, public ButtonListener, public SpinnerListener {
	private:
		RemoteSettings remoteSettings;
		u16 avatar[64*64];

		MainMenu2* menu;
		Button* backButton;
		Button* nextButton;
		Spinner* modeSpinner;
		TextField* ipField;
		TextField* portField;
		Spinner* gameSpinner;
		Spinner* routeSpinner;
		Spinner* characterSpinner;
		TextField* nameField;
		Spinner* chatSpinner;

		void OnSelectedGameChanged(GameInfo* gi);
		bool StartRemote();

	public:
		MultiPlayerScreen(GUI* gui, MainMenu2* menu);
		virtual ~MultiPlayerScreen();

		virtual void Activate();
		virtual void Deactivate();
		virtual void Cancel();

		virtual void OnSpinnerChanged(Spinner* spinner);
		virtual void OnButtonPressed(Button* button);
		virtual void DrawBackground();
		virtual void DrawForeground();
};

#endif
