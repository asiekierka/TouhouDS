#include "downloadscreen.h"

#include <ctime>
#include <vector>
#include <string>
#include "wifiscreen.h"
#include "../parser/csv_parser.h"
#include "../tcommon/wifi.h"
#include "../tcommon/parser/http.h"
#include "../tcommon/parser/ini_parser.h"
#include "../tcommon/gui/gui_common.h"

using namespace std;

//-----------------------------------------------------------------------------
DownloadScrollPane::DownloadScrollPane(Screen* screen, FontCache* fc)
: THTextScrollPane(screen, fc)
{
	repo = NULL;

	SetTextWrapping(false);
	SetTextBufferSize(4096);
	SetItemHeight(16);
	SetDefaultMargins(40, 8, 0, 0);
	SetFontSize(11, false);
}
DownloadScrollPane::~DownloadScrollPane() {

}

void DownloadScrollPane::Install(const char* path) {
	if (!repo) return;

	consoleClear();

#ifdef ENABLE_WIFI
	startWifi(&wifiConfig);
    printf("Trying to connect via Wifi.\nPress B to abort.\n");
    u32 assocStatus;
    do {
        swiWaitForVBlank();
        assocStatus = connectWifi();
        scanKeys();
    } while (assocStatus != ASSOCSTATUS_ASSOCIATED && !(keysDown() & KEY_B));

    if (keysDown() & KEY_B) {
    	printf("Aborted.\n");
    } else {
		printf("Connection Established.\n");
		printf("================================");

		for (u32 n = 0; n < markedForInstall.size(); n++) {
			const char* id = markedForInstall[n].c_str();
			RepositoryRecord* r = repo->GetRecord(id);
			if (r) {
				if (downloadFile(r->GetUrl(), "cache/download.tmp")) {
					printf("Download successful\n");
					if (unzip("cache/download.tmp", path)) {
						printf("Installation successful\n");

						//Unmark after successful installation
						markedForInstall.erase(markedForInstall.begin()+n);
						n--;
					} else {
						printf("Installation failed\n");
					}
				} else {
					printf("Error downloading: %s\n", repo->GetRecord(id)->GetUrl());
				}
			}
			printf("================================");
		}
    }

    stopWifi();
#endif

	printf("Press any key to continue...");
	waitForAnyKey();
	consoleClear();
}

void DownloadScrollPane::ToggleMark(int s) {
	if (!repo || s < 0 || s >= repo->GetNumberOfRecords()) {
		return;
	}

	const char* id = repo->GetRecord(s)->GetId();
	if (IsMarkedForInstall(id)) {
		for (u32 n = 0; n < markedForInstall.size(); n++) {
			if (strcmp(markedForInstall[n].c_str(), id) == 0) {
				markedForInstall.erase(markedForInstall.begin()+n);
				n--;
			}
		}
	} else {
		markedForInstall.push_back(string(id));
	}

}

bool DownloadScrollPane::OnTouch(u32& down, u32& held, touchPosition touch) {
	bool result = THTextScrollPane::OnTouch(down, held, touch);

	if (!result && (down & KEY_TOUCH)) {
		if (touch.px < 32) {
			ToggleMark(scroll + (touch.py - GetBounds().y) / listItemH);
			return true;
		}
	}
	return result;
}

void DownloadScrollPane::Update(u32& down, u32& held, touchPosition touch) {
	THTextScrollPane::Update(down, held, touch);

	int i = GetSelectedIndex();
	if (down & KEY_A) {
		down &= ~KEY_A;
		ToggleMark(i);
	}

	if (repo && i >= 0 && i < repo->GetNumberOfRecords()) {
		consoleClear();
		RepositoryRecord* r = repo->GetRecord(i);
		if (r) {
			char versionStr[32];
			r->GetVersionString(versionStr);

			char dateStr[32];
			u32 date = r->GetDate();
			struct tm* timeinfo = localtime((time_t*)&date);
			strftime(dateStr, 16, "%Y-%m-%d", timeinfo);

			printf("================================"
					"%s v%s\nauthor: %s\nlast edit: %s\n"
					"================================"
					"\n%s\n\n%s\n",
					r->GetId(), versionStr,
					r->GetAuthor(),
					dateStr,
					r->GetName(),
					r->GetDesc());
		}
	}
}

void DownloadScrollPane::DrawListItemForeground(s16 index, Rect r, bool selected) {
	THTextScrollPane::DrawListItemForeground(index, r, selected);

	if (!repo || index < 0 || index >= repo->GetNumberOfRecords()) {
		return;
	}
	RepositoryRecord* rec = repo->GetRecord(index);

	const int vs = VERTEX_SCALE(16);

	if (IsMarkedForInstall(rec->GetId())) {
		drawQuad(texture, inttof32(r.x), inttof32(r.y), vs, vs, Rect(48, 64, 16, 16));
	} else if (rec->GetTag() == RVT_installed
			|| rec->GetTag() == RVT_olderInstalled
			|| rec->GetTag() == RVT_newerInstalled)
	{
		drawQuad(texture, inttof32(r.x), inttof32(r.y), vs, vs, Rect(48, 48, 16, 16));
	} else {
		drawQuad(texture, inttof32(r.x), inttof32(r.y), vs, vs, Rect(48, 32, 16, 16));
	}

	//New/upgrade/downgrade icon
	if (rec->GetTag() == RVT_unknown) {
		drawQuad(texture, inttof32(r.x+16), inttof32(r.y), vs, vs, Rect(32, 32, 16, 16));
	} else if (rec->GetTag() == RVT_olderInstalled) {
		drawQuad(texture, inttof32(r.x+16), inttof32(r.y), vs, vs, Rect(32, 64, 16, 16));
	} else if (rec->GetTag() == RVT_newerInstalled) {
		drawQuad(texture, inttof32(r.x+16), inttof32(r.y), vs, vs, Rect(32, 48, 16, 16));
	}
}

bool DownloadScrollPane::IsMarkedForInstall(const char* id) {
	for (u32 n = 0; n < markedForInstall.size(); n++) {
		if (strcmp(markedForInstall[n].c_str(), id) == 0) {
			return true;
		}
	}
	return false;
}

void DownloadScrollPane::SetRepository(Repository* r) {
	if (repo != r) {
		repo = r;

		markedForInstall.clear();
		RemoveAllItems();
		if (r) {
			for (int n = 0; n < r->GetNumberOfRecords(); n++) {
				AppendText(r->GetRecord(n)->GetName());
			}

			SetScroll(0);
			SetSelectedIndex(0);
		}
	}
}

//-----------------------------------------------------------------------------
DownloadScreen::DownloadScreen(GUI* gui, MainMenu2* menu) : Screen(gui) {
	this->menu = menu;

	SetTexture(menu->textureI);
	SetBackgroundImage(menu->backgroundI+256*192);

	applyButton = new Button(this);
	applyButton->SetButtonListener(this);
	applyButton->SetForegroundImage(Rect(192, 96, 64, 16));
	applyButton->SetPressedImage(Rect(192, 112, 64, 16));
	applyButton->SetBounds(0, 176, 64, 16);
	applyButton->SetActivationKeys(KEY_START);

	gameButton = new Button(this);
	gameButton->SetButtonListener(this);
	gameButton->SetForegroundImage(Rect(64, 96, 64, 16));
	gameButton->SetPressedImage(Rect(64, 112, 64, 16));
	gameButton->SetBounds(64, 176, 64, 16);
	gameButton->SetActivationKeys(KEY_L);

	charaButton = new Button(this);
	charaButton->SetButtonListener(this);
	charaButton->SetForegroundImage(Rect(128, 96, 64, 16));
	charaButton->SetPressedImage(Rect(128, 112, 64, 16));
	charaButton->SetBounds(128, 176, 64, 16);
	charaButton->SetActivationKeys(KEY_R);

	wifiButton = new Button(this);
	wifiButton->SetButtonListener(this);
	wifiButton->SetForegroundImage(Rect(0, 96, 64, 16));
	wifiButton->SetPressedImage(Rect(0, 112, 64, 16));
	wifiButton->SetBounds(192, 176, 64, 16);
	wifiButton->SetActivationKeys(KEY_X);

	backButton = new Button(this);
	backButton->SetButtonListener(this);
	backButton->SetForegroundImage(Rect(64, 80, 48, 16));
	backButton->SetBounds(0, 0, 48, 16);
	backButton->SetActivationKeys(KEY_B);

	scrollPane = new DownloadScrollPane(this, defaultFontCache);
	scrollPane->SetItemBg(textureImage+(240<<8), 256, 16, textureImage+(240<<8), 256, 16);
	scrollPane->SetBounds(0, 16, 256, 160);

	mode = -1;
}
DownloadScreen::~DownloadScreen() {

}

void DownloadScreen::UpdateRepositories() {
	const char* downloadedRepo  = "cache/repo.tmp";
	const char* downloadedGames = "cache/games.tmp";
	const char* downloadedChara = "cache/chara.tmp";

	gameRepo.Clear();
    charaRepo.Clear();

#ifdef ENABLE_WIFI

    bool error = false;

    //Connect
	startWifi(&wifiConfig);
	printf("Trying to connect via Wifi.\n(Press B to abort)\n");
	u32 assocStatus;
	do {
		swiWaitForVBlank();
		assocStatus = connectWifi();
		scanKeys();
	} while (assocStatus != ASSOCSTATUS_ASSOCIATED && !(keysDown() & KEY_B));

	if (keysDown() & KEY_B) {
		printf("Aborted.\n");
	} else {
		//Download repo's
		printf("Connection Established.\n");
		printf("================================");

		CsvFile csvFile;
		if (!csvFile.Load("repo.txt")) {
			printf("Error opening repo.txt\n");
			error = true;
		} else {
			for (int n = 0; n < csvFile.GetNumberOfRecords(); n++) {
				CsvRecord* cr = csvFile.GetRecord(n);
				if (!cr || cr->GetNumberOfFields() < 2) {
					printf("Error parsing repo.txt\n");
					error = true;
					continue;
				}

				//const char* repoName = cr->AsString(0);
				const char* repoUrl = cr->AsString(1);

				char baseUrl[256];
				char gamesUrl[256];
				char charaUrl[256];
				baseUrl[0] = '\0';
				gamesUrl[0] = '\0';
				charaUrl[0] = '\0';

				sprintf(baseUrl, "%s/repo-" SCRIPT_VERSION "/repo.ini", repoUrl);
				if (!downloadFile(baseUrl, downloadedRepo)) {
					printf("Can't find repo at: %s\n", baseUrl);
					error = true;
					continue;
				}

				sprintf(baseUrl, "%s/repo-" SCRIPT_VERSION "/", repoUrl);

				IniFile iniFile;
				if (!iniFile.Load(downloadedRepo)) {
					printf("Error: Downloaded repo.ini invalid format\n");
					error = true;
					continue;
				}

				IniRecord* r;

				r = iniFile.GetRecord("version");
				if (r) {
					printf("%s=%s\n", r->name, r->value);

					if (strcmp(r->value, SCRIPT_VERSION) != 0) {
						printf("Error: Invalid repo version\n");
						error = true;
						continue;
					}
				}

				r = iniFile.GetRecord("games");
				if (r) {
					printf("%s=%s\n", r->name, r->value);
					toAbsoluteUrl(gamesUrl, baseUrl, r->value);
					downloadFile(gamesUrl, downloadedGames);
					gameRepo.AddAll(baseUrl, downloadedGames);
				}

				r = iniFile.GetRecord("chara");
				if (r) {
					printf("%s=%s\n", r->name, r->value);
					toAbsoluteUrl(charaUrl, baseUrl, r->value);
					downloadFile(charaUrl, downloadedChara);
					charaRepo.AddAll(baseUrl, downloadedChara);
				}
			}
		}
	}

	stopWifi();
	if (error) {
		printf("Some errors have occurred\n");
	}
	printf("================================");
	printf("Press any key to continue...");
	waitForAnyKey();
	consoleClear();

#else
	gameRepo.AddAll(downloadedRepo, downloadedGames);
    charaRepo.AddAll(downloadedRepo, downloadedChara);
#endif

    gameRepo.CompareWithInstall("games");
    charaRepo.CompareWithInstall("chara");
    scrollPane->SetRepository(mode == 0 ? &gameRepo : &charaRepo);
}

void DownloadScreen::OnButtonPressed(Button* button) {
	if (button == applyButton) {
		scrollPane->Install(mode == 0 ? "games" : "chara");
	} else if (button == gameButton) {
		mode = 0;
		gameButton->SetForegroundImage(Rect(64, 112, 64, 16));
		charaButton->SetForegroundImage(Rect(128, 96, 64, 16));
	    scrollPane->SetRepository(mode == 0 ? &gameRepo : &charaRepo);
	} else if (button == charaButton) {
		mode = 1;
		gameButton->SetForegroundImage(Rect(64, 96, 64, 16));
		charaButton->SetForegroundImage(Rect(128, 112, 64, 16));
	    scrollPane->SetRepository(mode == 0 ? &gameRepo : &charaRepo);
	} else if (button == wifiButton) {
		gui->PushScreen(new WifiScreen(gui, menu));
	} else if (button == backButton) {
		gui->PopScreen();
	}
}

void DownloadScreen::Update(u32& keysDown, u32& keysHeld, touchPosition touch) {
	Screen::Update(keysDown, keysHeld, touch);

    if (mode == -1) {
		UpdateRepositories();

		mode = 1;
		gameButton->SetForegroundImage(Rect(64, 96, 64, 16));
		charaButton->SetForegroundImage(Rect(128, 112, 64, 16));
		scrollPane->SetRepository(mode == 0 ? &gameRepo : &charaRepo);
    }
}

void DownloadScreen::Activate() {
	Screen::Activate();

	menu->soundManager.SetBGM(NULL);

	mode = -1;
    REG_BLDY_SUB = 12;
    REG_BLDCNT_SUB = BLEND_FADE_BLACK | BLEND_SRC_BG3;
}

void DownloadScreen::Deactivate() {
	Screen::Deactivate();

	scrollPane->SetRepository(NULL);
	consoleClear();

    REG_BLDY_SUB = 0;
    REG_BLDCNT_SUB = BLEND_NONE;
}
void DownloadScreen::Cancel() {
	Screen::Cancel();

	scrollPane->SetRepository(NULL);
	consoleClear();

    REG_BLDY_SUB = 0;
    REG_BLDCNT_SUB = BLEND_NONE;
}

void DownloadScreen::DrawForeground() {
	Screen::DrawForeground();

	drawQuad(texture, inttof32(0), inttof32(176), VERTEX_SCALE(256), VERTEX_SCALE(16),
			Rect(64, 224, 16, 16));
}

//-----------------------------------------------------------------------------
