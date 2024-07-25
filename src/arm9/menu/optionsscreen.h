#ifndef OPTIONSSCREEN_H
#define OPTIONSSCREEN_H

#include "mainmenu2.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"
#include "../tcommon/gui/spinner.h"

class OptionsScreen : public Screen, public ButtonListener, public SpinnerListener {
	private:
		MainMenu2* menu;
		Button* backButton;
		Spinner* fireButtonSpinner;
		Spinner* focusButtonSpinner;
		Spinner* bombButtonSpinner;
		Spinner* startLifeSpinner;
		Button* wifiButton;

	public:
		u16 dsButtonsI[96*16];
		u8  dsButtonsAlpha[96*16];

		OptionsScreen(GUI* gui, MainMenu2* menu);
		virtual ~OptionsScreen();

		virtual void OnButtonPressed(Button* button);
		virtual void OnSpinnerChanged(Spinner* spinner);

};

#endif
