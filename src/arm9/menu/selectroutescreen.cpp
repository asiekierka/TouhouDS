#include "selectroutescreen.h"
#include "selectcharacterscreen.h"
#include "thtextscrollpane.h"
#include "../gameinfo.h"
#include "../tcommon/text.h"
#include "../tcommon/gui/gui_common.h"

SelectRouteScreen::SelectRouteScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	leftButton = new Button(this);
	leftButton->SetButtonListener(this);
	leftButton->SetForegroundImage(Rect(0, 160, 32, 32));
	leftButton->SetPressedImage(Rect(64, 160, 32, 32));
	leftButton->SetBounds(16, 80, 32, 32);
	leftButton->SetActivationKeys(KEY_LEFT);

	rightButton = new Button(this);
	rightButton->SetButtonListener(this);
	rightButton->SetForegroundImage(Rect(32, 160, 32, 32));
	rightButton->SetPressedImage(Rect(96, 160, 32, 32));
	rightButton->SetBounds(208, 80, 32, 32);
	rightButton->SetActivationKeys(KEY_RIGHT);

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

	text = new Text();
	text->SetBuffer(128, 64);

	menu->selectedRoute = 0;
	OnSelectedRouteChanged();
}

SelectRouteScreen::~SelectRouteScreen() {
	delete text;
}

void SelectRouteScreen::OnSelectedRouteChanged() {
	SetBackgroundDirty();
}
void SelectRouteScreen::DrawBackground() {
	Screen::DrawBackground();

	darken(background, 1, 64, 64, 128, 64);

	GameInfo* gi = menu->GetSelectedGame();
	s8 r = menu->GetSelectedRoute();
	if (r >= 0 && r < gi->GetNumRoutes()) {
		text->ClearBuffer();
		text->SetFontSize(16);
		text->SetMargins(8, 8, 8, 8);
		text->PrintString(gi->GetRouteName(r));
		text->BlitToScreen(background, 256, 192, 0, 0, 64, 64, 128, 64);
	}
}
void SelectRouteScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen(GT_fade);
	} else if (button == nextButton) {
		gui->PushScreen(new SelectCharacterScreen(gui, menu), GT_fade);
	} else if (button == leftButton) {
		int sr = menu->selectedRoute - 1;
		if (sr < 0) sr += menu->GetSelectedGame()->GetNumRoutes();
		menu->selectedRoute = sr;
		OnSelectedRouteChanged();
	} else if (button == rightButton) {
		int sr = menu->selectedRoute + 1;
		if (sr >= menu->GetSelectedGame()->GetNumRoutes()) sr -= menu->GetSelectedGame()->GetNumRoutes();
		menu->selectedRoute = sr;
		OnSelectedRouteChanged();
	}
}

void SelectRouteScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(0), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));
	drawQuad(texture, inttof32(192), inttof32(176), VERTEX_SCALE(64), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));

	drawQuad(texture, inttof32(64), inttof32(176), VERTEX_SCALE(128), VERTEX_SCALE(16),
			Rect(128, 176, 128, 16));
}
