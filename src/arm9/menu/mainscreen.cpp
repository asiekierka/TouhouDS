#include "mainscreen.h"
#include "selectgamescreen.h"
#include "multiplayerscreen.h"
#include "optionsscreen.h"
#include "scorescreen.h"
#include "downloadscreen.h"
#include "practicescreen.h"
#include "../tcommon/gui/gui_common.h"
#include "../tcommon/text.h"

MainScreen::MainScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	singlePlayerButton = new Button(this);
	singlePlayerButton->SetButtonListener(this);
	singlePlayerButton->SetForegroundImage(Rect(64, 0, 96, 16));
	singlePlayerButton->SetBounds(152, 24, 96, 16);
	singlePlayerButton->SetActivationKeys(KEY_START|KEY_A);

	practiceButton = new Button(this);
	practiceButton->SetButtonListener(this);
	practiceButton->SetForegroundImage(Rect(64, 80, 96, 16));
	practiceButton->SetBounds(152, 56, 96, 16);

	multiPlayerButton = NULL;
	/*
	multiPlayerButton = new Button(this);
	multiPlayerButton->SetButtonListener(this);
	multiPlayerButton->SetForegroundImage(Rect(64, 16, 96, 16));
	multiPlayerButton->SetBounds(152, 56, 96, 16);
	*/

	wifiButton = new Button(this);
	wifiButton->SetButtonListener(this);
	wifiButton->SetForegroundImage(Rect(64, 64, 96, 16));
	wifiButton->SetBounds(152, 88, 96, 16);

	scoreButton = new Button(this);
	scoreButton->SetButtonListener(this);
	scoreButton->SetForegroundImage(Rect(64, 32, 96, 16));
	scoreButton->SetBounds(152, 120, 96, 16);

	configButton = new Button(this);
	configButton->SetButtonListener(this);
	configButton->SetForegroundImage(Rect(64, 48, 96, 16));
	configButton->SetBounds(152, 152, 96, 16);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 96, 96, 16));
	backButton->SetBounds(8, 192+8, 96, 16);
	backButton->SetActivationKeys(KEY_B);
}

MainScreen::~MainScreen() {

}

void MainScreen::Activate() {
	if (!background) {
		background = new u16[256*192];
	}

	Screen::Activate();

	SetTexture(menu->titleTextureI);
	SetBackgroundImage(menu->titleI+256*192);

	menu->soundManager.SetBGM("bgm/menu_bgm.wv");
}

void MainScreen::Deactivate() {
	Screen::Deactivate();

	SetTexture(NULL);
	SetBackgroundImage(NULL);

	if (background) {
		delete[] background;
		background = NULL;
	}
}

void MainScreen::Cancel() {
	menu->soundManager.SetBGM(NULL);

	Screen::Cancel();
}

void MainScreen::OnButtonPressed(Button* button) {
	if (button == singlePlayerButton) {
		gui->PushScreen(new SelectGameScreen(gui, menu), GT_fade);
	} else if (button == practiceButton) {
		gui->PushScreen(new PracticeScreen(gui, menu));
	} else if (button == multiPlayerButton) {
		gui->PushScreen(new MultiPlayerScreen(gui, menu));
	} else if (button == wifiButton) {
		gui->PushScreen(new DownloadScreen(gui, menu));
	} else if (button == scoreButton) {
		gui->PushScreen(new ScoreScreen(gui, menu));
	} else if (button == configButton) {
		gui->PushScreen(new OptionsScreen(gui, menu));
	} else if (button == backButton) {
		gui->PopScreen();
	}
}

void MainScreen::DrawBackground() {
	Screen::DrawBackground();

	const char* str = VERSION_STRING "\nCompiled on: " __DATE__;

	char string[512];
	string[0] = '\0';
#ifdef DEBUG
	sprintf(string, "Memory use: %dKB\n", HEAP_SIZE>>10);
#endif
	strcat(string, str);

	Text* text = new Text();
	text->SetBuffer(160, 96);
	text->SetFontSize(10);
	int lh = text->GetLineHeight();
	int lines = text->PrintString(string);
	text->BlitToScreen(background, 256, 192, 0, 0, 5, 192 - lh*(lines+1) - 5,
			text->GetBufferWidth(), lh*(lines+1));
	delete text;
}
