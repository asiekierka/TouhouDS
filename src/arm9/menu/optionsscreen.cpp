#include "optionsscreen.h"
#include "wifiscreen.h"
#include "../controls.h"
#include "../tcommon/gui/gui_common.h"

//------------------------------------------------------------------------------

class DSButtonSpinner : public Spinner {
	private:
		OptionsScreen* optionsScreen;

	public:
		DSButtonSpinner(OptionsScreen* screen) : Spinner (screen, Rect(32, 0, 16, 16),
				Rect(32, 16, 16, 16), Rect(48, 0, 16, 16), Rect(48, 16, 16, 16))
		{
			optionsScreen = screen;

		    AddChoice("A", KEY_A);
		    AddChoice("B", KEY_B);
		    AddChoice("X", KEY_X);
		    AddChoice("Y", KEY_Y);
		    AddChoice("L", KEY_L);
		    AddChoice("R", KEY_R);

		    SetBackgroundDirty();
		}
		virtual ~DSButtonSpinner() {
		}

		virtual void DrawBackground() {
			Rect r = GetBounds();
			if (!image && background) {
				int s = r.w * r.h;
				for (int n = 0; n < s; n++) {
					background[n] = BIT(15);
				}
			}

			Widget::DrawBackground();

			blitAlpha(optionsScreen->dsButtonsI, optionsScreen->dsButtonsAlpha, 96, 16,
					background, r.w, r.h, 16*selected, 0, (r.w-16)/2, 0, 16, 16);
		}
		virtual void OnButtonPressed(Button* button) {
			Spinner::OnButtonPressed(button);

			SetBackgroundDirty();
		}

};

//------------------------------------------------------------------------------

OptionsScreen::OptionsScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;
	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	loadImage("img/ds_buttons_16", dsButtonsI, dsButtonsAlpha, 96, 16, GL_RGB8_A5);

	fireButtonSpinner = new DSButtonSpinner(this);
	fireButtonSpinner->SetSelectedChoice(controls.fireButton);
	createLabeledWidget(this, "Fire", fireButtonSpinner, Rect(32, 64, 192, 16), 12);

	focusButtonSpinner = new DSButtonSpinner(this);
	focusButtonSpinner->SetSelectedChoice(controls.focusButton);
	createLabeledWidget(this, "Focus", focusButtonSpinner, Rect(32, 80, 192, 16), 12);

	bombButtonSpinner = new DSButtonSpinner(this);
	bombButtonSpinner->SetSelectedChoice(controls.bombButton);
	createLabeledWidget(this, "Bomb", bombButtonSpinner, Rect(32, 96, 192, 16), 12);

	startLifeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	startLifeSpinner->SetSpinnerListener(this);
	startLifeSpinner->SetNumeric(1, 10);
	startLifeSpinner->SetSelected(menu->startingLives + 1);
	createLabeledWidget(this, "Lives", startLifeSpinner, Rect(32, 120, 192, 16), 12);

	wifiButton = new Button(this);
	wifiButton->SetButtonListener(this);
	wifiButton->SetForegroundImage(Rect(0, 96, 64, 16));
	wifiButton->SetPressedImage(Rect(0, 112, 64, 16));
	wifiButton->SetBounds(96, 152, 64, 16);
	wifiButton->SetActivationKeys(KEY_X|KEY_A|KEY_START);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);
}

OptionsScreen::~OptionsScreen() {

}

void OptionsScreen::OnButtonPressed(Button* button) {
	if (button == wifiButton) {
		gui->PushScreen(new WifiScreen(gui, menu));
	} else if (button == backButton) {
		SpinnerChoice* choice;

		choice = fireButtonSpinner->GetChoiceValue();
		if (choice) controls.fireButton = choice->value;
		choice = focusButtonSpinner->GetChoiceValue();
		if (choice) controls.focusButton = choice->value;
		choice = bombButtonSpinner->GetChoiceValue();
		if (choice) controls.bombButton = choice->value;

		controls.Save();

		gui->PopScreen();
	}
}

void OptionsScreen::OnSpinnerChanged(Spinner* spinner) {
	if (spinner == startLifeSpinner) {
		menu->startingLives = startLifeSpinner->GetNumericValue() - 1;
	}
}
