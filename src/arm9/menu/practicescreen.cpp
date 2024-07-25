#include "practicescreen.h"
#include "selectcharacterscreen.h"
#include "thtextscrollpane.h"
#include "../charinfo.h"
#include "../gameinfo.h"
#include "../tcommon/gui/gui_common.h"
#include "../tcommon/parser/ini_parser.h"

PracticeScreen::PracticeScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;
	SetTexture(menu->textureI);
	//SetBackgroundImage(menu->backgroundI+256*192);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);

	nextButton = new Button(this);
	nextButton->SetButtonListener(this);
	nextButton->SetForegroundImage(Rect(160, 80, 48, 16));
	nextButton->SetBounds(208, 0, 48, 16);
	nextButton->SetActivationKeys(KEY_A|KEY_START);

	int cy = 0;

	Label* gameLabel = new Label(this, "Game", 12);
	gameLabel->SetBounds(0, cy+16, 64, 16);
	gameSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	for (int n = 0; n < gameInfoL; n++) {
		if (gameInfo[n]) {
			gameSpinner->AddChoice(gameInfo[n]->GetName(), n);
		}
	}
	gameSpinner->SetSpinnerListener(this);
	gameSpinner->SetBounds(64, cy+16, 192, 16);

	Label* routeLabel = new Label(this, "Route", 12);
	routeLabel->SetBounds(0, cy+32, 64, 16);
	routeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	routeSpinner->SetSpinnerListener(this);
	routeSpinner->SetBounds(64, cy+32, 192, 16);

	stagePane = new THTextScrollPane(this, defaultFontCache);
	stagePane->SetItemBg(textureImage+(240<<8), 256, 16, textureImage+(240<<8), 256, 16);
	stagePane->SetTextWrapping(false);
	stagePane->SetTextBufferSize(1024);
	stagePane->SetItemHeight(16);
	stagePane->SetDefaultMargins(8, 8, 0, 0);
	stagePane->SetFont("mono.ttf");
	stagePane->SetFontSize(10, false);
	stagePane->SetBounds(0, 64, 256, 128);
	stagePane->SetShowSelections(true);

	OnSpinnerChanged(gameSpinner);
}

PracticeScreen::~PracticeScreen() {
}

void PracticeScreen::Deactivate() {
	menu->selectedStage = stagePane->GetSelectedIndex() + 1;
	menu->practice = true;

	Screen::Deactivate();
}

void PracticeScreen::Cancel() {
	menu->practice = false;

	Screen::Cancel();
}

void PracticeScreen::OnSpinnerChanged(Spinner* spinner) {
	GameInfo* gi = menu->selectedGame >= 0 ? gameInfo[menu->selectedGame] : NULL;

	SpinnerChoice* sc;

	sc = gameSpinner->GetChoiceValue();
	menu->selectedGame = (sc ? sc->value : -1);
	if (spinner == gameSpinner) {
		OnSelectedGameChanged(gi);
	}

	sc = routeSpinner->GetChoiceValue();
	menu->selectedRoute = (sc ? sc->value : -1);
	if (spinner == routeSpinner) {
		OnSelectedRouteChanged(gi, menu->selectedRoute);
	}
}

void PracticeScreen::OnSelectedGameChanged(GameInfo* gi) {
	routeSpinner->RemoveAll();
	if (gi) {
		for (int n = 0; n < gi->GetNumRoutes(); n++) {
			routeSpinner->AddChoice(gi->GetRouteName(n), n);
		}
	}

	OnSelectedRouteChanged(gi, routeSpinner->GetNumericValue());
}

void PracticeScreen::OnSelectedRouteChanged(GameInfo* gi, int route) {
	stagePane->RemoveAllItems();
	if (gi && route >= 0 && route < gi->GetNumRoutes()) {
	    IniFile iniFile;
	    char temp[PATH_MAX];

	    for (int n = 0; n <= 64; n++) {
	    	bool error = false;

			sprintf(temp, "games/%s/%s/stage%d.ini", gi->GetId(), gi->GetRouteId(route), n);
			if (iniFile.Load(temp)) {
				IniRecord* record;

				record = iniFile.GetRecord("name");
				const char* name = (record ? record->AsString() : "");
				record = iniFile.GetRecord("desc");
				const char* desc = (record ? record->AsString() : "");

				sprintf(temp, "%2d.  %s: %s", n, name, desc);
			} else {
				sprintf(temp, "%2d.  ERROR", n);
				error = true;
			}

			if (!error) {
				stagePane->AppendText(temp, RGB15(31, 31, 31), false);
			} else if (n > 0) {
				break;
			}
	    }

		//TODO: Change stage spinner to the number of stages for this route.
	}
}

void PracticeScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen();
	} else if (button == nextButton) {
		gui->PushScreen(new SelectCharacterScreen(gui, menu), GT_fade);
	}
}

void PracticeScreen::DrawBackground() {
	Screen::DrawBackground();

	for (int n = 0; n < 16; n++) {
		blit(textureImage, 256, 256, background, 256, 192, 80, 224, 16*n, 48, 16, 16);
	}
}

void PracticeScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(48), inttof32(0), VERTEX_SCALE(208), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));
}
