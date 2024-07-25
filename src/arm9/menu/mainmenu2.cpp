#include "mainmenu2.h"
#include "titlescreen.h"

#include "../as_lib9.h"
#include "../bombinfo.h"
#include "../charinfo.h"
#include "../controls.h"
#include "../gameinfo.h"
#include "../tcommon/gui/gui_common.h"
#include "../tcommon/parser/ini_parser.h"

MainMenu2::MainMenu2() {
	quit = false;
	gui = new GUI();

	titleI = titleTextureI = NULL;
	textureI = new u16[256*256];
	backgroundI = new u16[256*384];

	soundManager.Init("snd/menu", 64*1024);
}
MainMenu2::~MainMenu2() {
	delete gui;

	UnloadTitleImages();
	delete[] textureI;
	delete[] backgroundI;
}

void MainMenu2::Run() {
	selectedCharacter = 0;
	selectedGame = 0;
	selectedRoute = 0;
	selectedStage = 1;
	practice = false;
	remotePlay = false;
	startingLives = 2;
	quit = false;

#ifdef DEBUG
	startingLives = 9;
#endif

	LoadTitleImages();

	loadImage("img/menu_tex", textureI, NULL, 256, 256, GL_RGBA);
	loadImage("img/menu_bg", backgroundI, NULL, 256, 384, GL_RGB256);
	DC_FlushRange(textureI, 256*256*2);
	DC_FlushRange(backgroundI, 256*384*2);

	gui->VideoInit();
	gui->PushScreen(new TitleScreen(gui, this));

	//Init stuff
    controls.Load();
    initBombInfo();
    initCharacterInfo();
    initGameInfo();

    gui->Draw();
	unfadeBlack2(16);

	bool quickstart = false;
	#ifdef DEBUG
		IniFile iniFile;
		if (iniFile.Load("debug.ini")) {
			IniRecord* r;

			r = iniFile.GetRecord("quickstart");
			if (r) quickstart = (r->AsInt() != 0);
			r = iniFile.GetRecord("game");
			if (r) selectedGame = r->AsInt();
			r = iniFile.GetRecord("route");
			if (r) selectedRoute = r->AsInt();
			r = iniFile.GetRecord("stage");
			if (r) selectedStage = r->AsInt();
			r = iniFile.GetRecord("chara");
			if (r) selectedCharacter = r->AsInt();

			selectedGame = MAX(0, MIN(gameInfoL-1, selectedGame));
			selectedRoute = MAX(0, MIN(GetSelectedGame()->GetNumRoutes()-1, selectedRoute));
			selectedStage = MAX(1, selectedStage);
			selectedCharacter = MAX(0, MIN(charInfoL-1, selectedCharacter));
		}
	#endif

	while (!quit) {
		gui->Update();
		gui->Draw();

		soundManager.Update();
		swiWaitForVBlank();

		if (quickstart) break;
	}
	soundManager.SetBGM(NULL);
}

void MainMenu2::LoadTitleImages() {
	titleI = new u16[256*384];
	titleTextureI = new u16[256*256];
	loadImage("img/title", titleI, NULL, 256, 384, GL_RGB256);
	loadImage("img/menu_title_tex", titleTextureI, NULL, 256, 256, GL_RGBA);
	DC_FlushRange(titleI, 256*192*2);
	DC_FlushRange(titleTextureI, 256*256*2);
}

void MainMenu2::UnloadTitleImages() {
	if (titleI) {
		delete[] titleI;
		titleI = NULL;
	}
	if (titleTextureI) {
		delete[] titleTextureI;
		titleTextureI = NULL;
	}
}

void MainMenu2::Quit() {
	quit = true;
}

CharInfo* MainMenu2::GetSelectedCharacter() {
	if (selectedCharacter >= 0 && selectedCharacter < charInfoL) {
		return &charInfo[selectedCharacter];
	}
	return NULL;
}
GameInfo* MainMenu2::GetSelectedGame() {
	if (selectedGame >= 0 && selectedGame < gameInfoL) {
		return gameInfo[selectedGame];
	}
	return NULL;
}

s8 MainMenu2::GetSelectedRoute() {
	return selectedRoute;
}

s8 MainMenu2::GetSelectedStage() {
	return selectedStage;
}
