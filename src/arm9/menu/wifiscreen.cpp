#include "wifiscreen.h"

#include <dswifi9.h>
#include "../tcommon/gui/gui_common.h"

WifiScreen::WifiScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	Label* ssidLabel = new Label(this, "SSID", 12);
	ssidLabel->SetBounds(0, 16, 96, 16);
	ssidField = new TextField(this, 10);
	ssidField->SetBounds(96, 16, 160, 16);

	Label* wepLabel = new Label(this, "WEP", 12);
	wepLabel->SetBounds(0, 48, 96, 16);
	wepTypeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	wepTypeSpinner->SetSpinnerListener(this);
	wepTypeSpinner->SetBounds(96, 48, 160, 16);
	wepTypeSpinner->AddChoice("off", WEPMODE_NONE);
	wepTypeSpinner->AddChoice("40 bit", WEPMODE_40BIT);
	wepTypeSpinner->AddChoice("128 bit", WEPMODE_128BIT);

	Label* wepKeyTypeLabel = new Label(this, "WEP Key Type", 12);
	wepKeyTypeLabel->SetBounds(0, 64, 96, 16);
	wepKeyTypeSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	wepKeyTypeSpinner->SetSpinnerListener(this);
	wepKeyTypeSpinner->SetBounds(96, 64, 160, 16);
	wepKeyTypeSpinner->AddChoice("Hex", 0);
	wepKeyTypeSpinner->AddChoice("ASCII", 1);

	Label* wepKeyLabel = new Label(this, "WEP Key", 12);
	wepKeyLabel->SetBounds(0, 80, 96, 16);
	wepKeyField = new TextField(this, 10);
	wepKeyField->SetBounds(96, 80, 160, 16);

	Label* ipConfigLabel = new Label(this, "IP Config", 12);
	ipConfigLabel->SetBounds(0, 112, 96, 16);
	ipConfigSpinner = new Spinner(this, Rect(32, 0, 16, 16), Rect(32, 16, 16, 16),
			Rect(48, 0, 16, 16), Rect(48, 16, 16, 16));
	ipConfigSpinner->SetSpinnerListener(this);
	ipConfigSpinner->SetBounds(96, 112, 160, 16);
	ipConfigSpinner->AddChoice("auto", 1);
	ipConfigSpinner->AddChoice("manual", 0);

	Label* ipLabel = new Label(this, "IP", 12);
	ipLabel->SetBounds(0, 128, 96, 16);
	ipField = new TextField(this, 10);
	ipField->SetBounds(96, 128, 160, 16);

	Label* subnetMaskLabel = new Label(this, "Subnet Mask", 12);
	subnetMaskLabel->SetBounds(0, 144, 96, 16);
	subnetMaskField = new TextField(this, 10);
	subnetMaskField->SetBounds(96, 144, 160, 16);

	Label* gatewayLabel = new Label(this, "Gateway", 12);
	gatewayLabel->SetBounds(0, 160, 96, 16);
	gatewayField = new TextField(this, 10);
	gatewayField->SetBounds(96, 160, 160, 16);

	Label* dnsLabel = new Label(this, "DNS", 12);
	dnsLabel->SetBounds(0, 176, 96, 16);
	dnsField = new TextField(this, 10);
	dnsField->SetBounds(96, 176, 160, 16);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);

#ifdef ENABLE_WIFI
	testButton = new Button(this);
	testButton->SetButtonListener(this);
	testButton->SetForegroundImage(Rect(128, 128, 64, 16));
	testButton->SetBounds(192, 0, 64, 16);
	testButton->SetActivationKeys(KEY_START);
#endif

	char temp[64];

	strncpy(temp, wifiConfig.ap.ssid, 33);
	temp[33] = '\0';
	ssidField->SetText(temp);

	wepTypeSpinner->SetSelected(wifiConfig.wepMode);
	wepKeyTypeSpinner->SetSelected(wifiConfig.wepKeyType);

	strncpy(temp, (const char*)wifiConfig.wepKey, 16);
	temp[17] = '\0';
	wepKeyField->SetText(temp);

	ipConfigSpinner->SetSelected(wifiConfig.dhcp ? 0 : 1);
	ipField->SetText(ipToString(temp, wifiConfig.ip));
	subnetMaskField->SetText(ipToString(temp, wifiConfig.subnetMask));
	gatewayField->SetText(ipToString(temp, wifiConfig.gateway));
	dnsField->SetText(ipToString(temp, wifiConfig.dns1));
}
WifiScreen::~WifiScreen() {
}

void WifiScreen::DrawBackground() {
	Screen::DrawBackground();

	int y[] = {32, 96};
	for (int t = 0; t < 2; t++) {
		for (int n = 0; n < 16; n++) {
			blit(textureImage, 256, 256, background, 256, 192, 80, 224, 16*n, y[t], 16, 16);
		}
	}
}

void WifiScreen::Update(u32& down, u32& held, touchPosition touch) {
	Screen::Update(down, held, touch);

	/*
	if (down & KEY_L) {
		WifiTest* wtest = new WifiTest(&wifiConfig);
		wtest->Run();
		delete wtest;
		consoleClear();
	}
	*/
}

void WifiScreen::OnSpinnerChanged(Spinner* spinner) {
}
void WifiScreen::OnButtonPressed(Button* button) {
	if (button == backButton) {
		gui->PopScreen();
#ifdef ENABLE_WIFI
	} else if (button == testButton) {
		const u8 frames = 8;
	    REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;
	    for (u16 n = 0; n <= frames; n++) {
	    	REG_BLDY_SUB = 0x1F * n / frames;
	        swiWaitForVBlank();
	    }

		startWifi(&wifiConfig);
	    iprintf("Trying to connect via Wifi.\nPress B to abort.\n");
	    u32 assocStatus;
	    do {
	        swiWaitForVBlank();
	        assocStatus = connectWifi();
	        scanKeys();
	    } while (assocStatus != ASSOCSTATUS_ASSOCIATED && !(keysDown() & KEY_B));

	    if (keysDown() & KEY_B) {
	    	iprintf("Aborted.\n");
	    } else {
			iprintf("Connection Established.\n");
			iprintf("Trying to connect to www.google.com\n");

			TCPConnection con;
		    if (con.Connect("www.google.com") < 0) {
				iprintf("Website unreachable...\n");
		    } else {
				iprintf("Connected to www.google.com successfully...\n");
		    }
	    }

	    stopWifi();
		iprintf("Press any key to continue...");
		waitForAnyKey();
		consoleClear();

	    for (u16 n = 0; n <= frames; n++) {
	    	REG_BLDY_SUB = 0x1F - 0x1F * n / frames;
	        swiWaitForVBlank();
	    }
	    REG_BLDCNT_SUB = BLEND_NONE;
#endif
	}
}

void WifiScreen::UpdateWifiConfig() {
	strncpy(wifiConfig.ap.ssid, ssidField->GetText(), 33);
	wifiConfig.ap.ssid_len = MIN(33, strlen(ssidField->GetText()));
	strncpy((char*)wifiConfig.wepKey, wepKeyField->GetText(), 16);
	wifiConfig.wepKeyType = (WepKeyType)wepKeyTypeSpinner->GetChoiceValue()->value;
	wifiConfig.wepMode = wepTypeSpinner->GetChoiceValue()->value;
	wifiConfig.dhcp = ipConfigSpinner->GetChoiceValue()->value;
	wifiConfig.ip = stringToIP(ipField->GetText());
	wifiConfig.subnetMask = stringToIP(subnetMaskField->GetText());
	wifiConfig.gateway = stringToIP(gatewayField->GetText());
	wifiConfig.dns1 = stringToIP(dnsField->GetText());
}

void WifiScreen::Cancel() {
	Screen::Deactivate();

	UpdateWifiConfig();
	wifiConfig.Save("wifi.ini");
}
void WifiScreen::Deactivate() {
	Screen::Deactivate();

	UpdateWifiConfig();
	wifiConfig.Save("wifi.ini");
}
void WifiScreen::Activate() {
	Screen::Activate();

	wifiConfig.Load("wifi.ini");
	menu->soundManager.SetBGM(NULL);
}
