#include "scorescreen.h"

#include <time.h>
#include "thtextscrollpane.h"
#include "../controls.h"
#include "../charinfo.h"
#include "../gameinfo.h"
#include "../tcommon/gui/gui_common.h"

ScoreScreen::ScoreScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	Label* gameLabel = new Label(this, "Game", 12);
	gameLabel->SetBounds(0, 16, 64, 16);
	gameSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	for (int n = 0; n < gameInfoL; n++) {
		if (gameInfo[n]) {
			gameSpinner->AddChoice(gameInfo[n]->GetName(), n);
		}
	}
	gameSpinner->SetSpinnerListener(this);
	gameSpinner->SetBounds(64, 16, 192, 16);

	Label* routeLabel = new Label(this, "Route", 12);
	routeLabel->SetBounds(0, 32, 64, 16);
	routeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	routeSpinner->SetSpinnerListener(this);
	routeSpinner->SetBounds(64, 32, 192, 16);

	Label* characterLabel = new Label(this, "Chara", 12);
	characterLabel->SetBounds(0, 48, 64, 16);
	characterSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	for (int n = 0; n < charInfoL; n++) {
		characterSpinner->AddChoice(charInfo[n].GetName(), n);
	}
	characterSpinner->SetSpinnerListener(this);
	characterSpinner->SetBounds(64, 48, 192, 16);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);

	scorePane = new THTextScrollPane(this, defaultFontCache);
	scorePane->SetItemBg(textureImage+(240<<8), 256, 16, textureImage+(240<<8), 256, 16);
	scorePane->SetTextWrapping(false);
	scorePane->SetTextBufferSize(1024);
	scorePane->SetItemHeight(16);
	scorePane->SetDefaultMargins(8, 8, 0, 0);
	scorePane->SetFont("mono.ttf");
	scorePane->SetFontSize(10, false);
	scorePane->SetBounds(0, 80, 256, 112);
	scorePane->SetShowSelections(true);

	OnSpinnerChanged(gameSpinner);
}

ScoreScreen::~ScoreScreen() {

}

void ScoreScreen::DrawBackground() {
	Screen::DrawBackground();

	for (int n = 0; n < 16; n++) {
		blit(textureImage, 256, 256, background, 256, 192, 80, 224, 16*n, 64, 16, 16);
	}
}

void ScoreScreen::OnSpinnerChanged(Spinner* spinner) {
	SpinnerChoice* sc;
	GameInfo* gi = NULL;

	sc = gameSpinner->GetChoiceValue();
	if (sc) {
		gi = gameInfo[sc->value];
	}

	if (spinner == gameSpinner) {
		OnSelectedGameChanged(gi);
	}

	scorePane->RemoveAllItems();

    sc = routeSpinner->GetChoiceValue();
    int scoreRoute = (sc ? sc->value : -1);
    sc = characterSpinner->GetChoiceValue();
    int scoreChara = (sc ? sc->value : -1);

    ScoreRecord* scores = new ScoreRecord[10];
    int scoresL = 0;
    if (scoreRoute >= 0 && gi && scoreRoute < gi->GetNumRoutes()) {
        if (scoreChara >= 0 && scoreChara < charInfoL) {
            scoresL = gi->GetScoreRecords(scores, gi->GetRouteId(scoreRoute),
            		charInfo[scoreChara].GetId());
        }
    }

    char str[256];
    char dateStr[16];
    char stageStr[16];
    for (int n = 0; n < scoresL; n++) {
        struct tm* timeinfo = localtime((time_t*)&scores[n].timestamp);
        strftime(dateStr, 16, "%Y-%m-%d", timeinfo);

        if (scores[n].stage >= 100) {
            sprintf(stageStr, "complete");
        } else {
            sprintf(stageStr, "stage %-2d", MIN(99, scores[n].stage));
        }

        sprintf(str, "%2d.  %s  %s   %9d\n", n+1, dateStr, stageStr, MIN(999999999, scores[n].score));
        scorePane->AppendText(str, RGB15(31,31,31), false);
    }

    delete[] scores;

    scorePane->SetScroll(0);
    scorePane->SetSelectedIndex(0);
}
void ScoreScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen();
	}
}
void ScoreScreen::OnSelectedGameChanged(GameInfo* gi) {
	routeSpinner->RemoveAll();
	if (gi) {
		for (int n = 0; n < gi->GetNumRoutes(); n++) {
			routeSpinner->AddChoice(gi->GetRouteName(n), n);
		}
	}
}
