#include "multiplayerscreen.h"
#include "../charinfo.h"
#include "../gameinfo.h"
#include "../remote/remote.h"
#include "../tcommon/wifi.h"
#include "../tcommon/gui/gui_common.h"

#define CONFIG_FILE "multiplayer/remote.ini"

MultiPlayerScreen::MultiPlayerScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
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

	int cy = 16;

	Label* modeLabel = new Label(this, "Mode", 12);
	modeLabel->SetBounds(0, cy+0, 64, 16);
	modeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	modeSpinner->SetSpinnerListener(this);
	modeSpinner->SetBounds(64, cy+0, 192, 16);
	modeSpinner->AddChoice("Join existing game", 0);
	modeSpinner->AddChoice("Host new game", 1);

	cy = 48;

	Label* ipLabel = new Label(this, "Server IP", 12);
	ipLabel->SetBounds(0, cy+0, 74, 16);
	ipField = new TextField(this, 10);
	ipField->SetBounds(74, cy+0, 90, 16);

	Label* portLabel = new Label(this, "Port", 12);
	portLabel->SetBounds(164, cy+0, 42, 16);
	portField = new TextField(this, 10);
	portField->SetBounds(206, cy+0, 50, 16);

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

	cy = 128;

	Label* nameLabel = new Label(this, "Nick", 12);
	nameLabel->SetBounds(64, cy+0, 64, 16);
	nameField = new TextField(this, 10);
	nameField->SetBounds(128, cy+0, 128, 16);
	nameField->SetLimit(NICKNAME_LENGTH);

	Label* characterLabel = new Label(this, "Chara", 12);
	characterLabel->SetBounds(64, cy+16, 64, 16);
	characterSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	for (int n = 0; n < charInfoL; n++) {
		characterSpinner->AddChoice(charInfo[n].GetName(), n);
	}
	characterSpinner->SetSpinnerListener(this);
	characterSpinner->SetBounds(128, cy+16, 128, 16);

	Label* chatLabel = new Label(this, "Chat", 12);
	chatLabel->SetBounds(64, cy+32, 64, 16);
	chatSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	chatSpinner->SetSpinnerListener(this);
	chatSpinner->SetBounds(128, cy+32, 128, 16);
	chatSpinner->AddChoice("Off", 0);
	chatSpinner->AddChoice("Receive Only", 1);
	chatSpinner->AddChoice("Send & Receive", 3);

	OnSpinnerChanged(gameSpinner);
	OnSpinnerChanged(modeSpinner);
}

MultiPlayerScreen::~MultiPlayerScreen() {
}

void MultiPlayerScreen::Activate() {
	Screen::Activate();

	if (!remoteSettings.Load(CONFIG_FILE)) {
		remoteSettings.Reset();
	}

	char temp[32];
	ipField->SetText(ipToString(temp, remoteSettings.ip));
	sprintf(temp, "%d", remoteSettings.port);
	portField->SetText(temp);
	nameField->SetText(remoteSettings.nickname);
	chatSpinner->SetSelectedChoice(
			(remoteSettings.receiveVoice ? 1 : 0)
			| (remoteSettings.sendVoice ? 2 : 0));

	if (!loadImage("multiplayer/avatar", avatar, NULL, 64, 64, 0)) {
		memset(avatar, 0, 64*64*sizeof(u16));
	}

    REG_BLDY_SUB = 12;
    REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;
}

void cancel(RemoteSettings* rs, const char* nickname) {
	strncpy(rs->nickname, nickname, NICKNAME_LENGTH);
	rs->Save(CONFIG_FILE);

	consoleClear();
    REG_BLDY_SUB = 0;
    REG_BLDCNT_SUB = BLEND_NONE;
}

void MultiPlayerScreen::Deactivate() {
	Screen::Deactivate();

	cancel(&remoteSettings, nameField->GetText());
}
void MultiPlayerScreen::Cancel() {
	Screen::Cancel();

	cancel(&remoteSettings, nameField->GetText());
}

void MultiPlayerScreen::OnSpinnerChanged(Spinner* spinner) {
	SpinnerChoice* sc;

	sc = gameSpinner->GetChoiceValue();
	menu->selectedGame = (sc ? sc->value : -1);

	if (spinner == gameSpinner) {
		OnSelectedGameChanged(menu->selectedGame >= 0 ? gameInfo[menu->selectedGame] : NULL);
	}

	sc = routeSpinner->GetChoiceValue();
	menu->selectedRoute = (sc ? sc->value : -1);
	sc = characterSpinner->GetChoiceValue();
	menu->selectedCharacter = (sc ? sc->value : -1);

	sc = chatSpinner->GetChoiceValue();
	if (sc) {
		int val = sc->value;
		remoteSettings.receiveVoice = (val & 0x1);
		remoteSettings.sendVoice = (val & 0x2);
	}

	if (spinner == modeSpinner) {
		SpinnerChoice* sc = modeSpinner->GetChoiceValue();
		bool v = (sc ? sc->value == 1 : false);

		ipField->SetVisible(!v);
		ipField->SetBackgroundDirty();
		gameSpinner->SetVisible(v);
		gameSpinner->SetBackgroundDirty();
		routeSpinner->SetVisible(v);
		gameSpinner->SetBackgroundDirty();
	}
}

void MultiPlayerScreen::OnSelectedGameChanged(GameInfo* gi) {
	routeSpinner->RemoveAll();
	if (gi) {
		for (int n = 0; n < gi->GetNumRoutes(); n++) {
			routeSpinner->AddChoice(gi->GetRouteName(n), n);
		}
	}
}

bool MultiPlayerScreen::StartRemote() {

#ifndef ENABLE_WIFI
	iprintf("Error: program compiled without Wifi support\n");
	waitForAnyKey();
	return false;
#endif

	iprintf("Trying to connect via Wifi.\nPress B to abort.\n");

	startWifi(&wifiConfig);
	u32 assocStatus;
	do {
		swiWaitForVBlank();
		assocStatus = connectWifi();
		scanKeys();
	} while (assocStatus != ASSOCSTATUS_ASSOCIATED && !(keysDown() & KEY_B));

	if (keysDown() & KEY_B) {
		iprintf("Aborted.\n");
		stopWifi();
		return false;
	}

	iprintf("Connection Established.\n");
	iprintf("================================");

	if (modeSpinner->GetChoiceValue()->value == 1) {
		iprintf("Starting server...\n");
		remote.StartServer();
	} else {
		iprintf("Connecting to server...\n");
		remote.StartClient(stringToIP(ipField->GetText()));
	}

	iprintf("Waiting for other players.\n");
	do {
		remote.UpdatePreGame();
		scanKeys();
	} while(!(keysHeld() & KEY_X));

	iprintf("Starting Game...\n");
	return true;
}

void MultiPlayerScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen();
	} else if (button == nextButton) {
	    bool ok = StartRemote();
		consoleClear();

	    if (ok) {
	    	menu->remotePlay = true;
			menu->Quit();
		}
	}
}

void MultiPlayerScreen::DrawBackground() {
	Screen::DrawBackground();

	int y[] = {32, 112};
	for (int t = 0; t < 2; t++) {
		for (int n = 0; n < 16; n++) {
			blit(textureImage, 256, 256, background, 256, 192, 80, 224, 16*n, y[t], 16, 16);
		}
	}

	blit(avatar, 64, 64, background, 256, 192, 0, 0, 0, 128, 64, 64);
}

void MultiPlayerScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(48), inttof32(0), VERTEX_SCALE(208), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));

	SpinnerChoice* sc = modeSpinner->GetChoiceValue();
	bool v = (sc ? sc->value == 1 : false);
	glTranslate3f32(0, 0, 1);
	if (v) {
		drawQuad(texture, inttof32(0), inttof32(48), VERTEX_SCALE(164), VERTEX_SCALE(16),
				Rect(0, 192, 16, 16));
	}
	if (!v) {
		drawQuad(texture, inttof32(0), inttof32(64), VERTEX_SCALE(256), VERTEX_SCALE(32),
				Rect(0, 192, 16, 16));
	}
	glTranslate3f32(0, 0, -1);
}
