#include <nds.h>
#include <fat.h>
#include <unistd.h>
#include <sys/dir.h>

#include "efs_lib.h"
#include "../common/fifo.h"

#include "thcommon.h"
#include "as_lib9.h"

#include "game.h"
#include "charinfo.h"
#include "gameinfo.h"
#include "remote/remote.h"
#include "spells/spell_decls.h"
#include "menu/mainmenu2.h"
#include "menu/scripterrorscreen.h"

#include "tcommon/text.h"
#include "tcommon/wifi.h"
#include "tcommon/gui/gui_common.h"

void epicFail() {
	lcdMainOnBottom();
	consoleDemoInit();

    printf("\x1b[0;0H");
    BG_PALETTE_SUB[0]   = RGB15( 0,  0, 31) | BIT(15);
    BG_PALETTE_SUB[255] = RGB15(31, 31, 31) | BIT(15);
}

void libfatFail() {
    epicFail();

    const char* warning =
    "--------------------------------"
    "         libfat failure         "
    "--------------------------------"
    "                                "
    " An error is preventing the     "
    " game from accessing external   "
    " files.                         "
    "                                "
    " If you're using an emulator,   "
    " make sure it supports DLDI     "
    " and that it's activated.       "
    "                                "
    " In case you're seeing this     "
    " screen on a real DS, make sure "
    " you've applied the correct     "
    " DLDI patch (most modern        "
    " flashcards do this             "
    " automatically)                 "
    "                                "
    " Note: Some cards only          "
    " autopatch .nds files stored in "
    " the root folder of the card.   "
    "                                "
    "--------------------------------";
    iprintf(warning);

    while (true) {
        swiWaitForVBlank();
    }
}

void installFail() {
    epicFail();

    const char* warning =
    "--------------------------------"
    "      Invalid Installation      "
    "--------------------------------"
    "                                "
    " Please make sure the game data "
    " is stored in /TouhouDS/        "
    "                                "
    "--------------------------------";
    iprintf(warning);

    while (true) {
        swiWaitForVBlank();
    }
}

void checkInstall() {
	if (!fexists("font.ttf") || !fexists("mono.ttf")
			|| !fexists("img/title.dta") || !fexists("img/title.pal"))
	{
        installFail();
	}
}

void scriptFail(int code) {
	resetVideo();

	u16* textureI = new u16[256*256];
	loadImage("img/menu_tex", textureI, NULL, 256, 256, GL_RGBA);

	GUI* gui = new GUI();
	gui->VideoInit();

	ScriptErrorScreen* es = new ScriptErrorScreen(gui, textureI);
	gui->PushScreen(es);
	es->SetError(code);

	while (gui->GetActiveScreen()) {
		gui->Update();
		gui->Draw();

		AS_SoundVBL();
		swiWaitForVBlank();

		if (keysDown() & (KEY_A|KEY_B|KEY_START)) {
			break;
		}
	}
	delete gui;
	delete[] textureI;

    resetVideo();
}

int main(int argc, char** argv) {
	powerOn(POWER_ALL);
    defaultExceptionHandler();

    for (int n = 0; n < 6; n++) {
    	//Wait for ARM7 init
    	swiWaitForVBlank();
    }

    if (!fifoSetValue32Handler(TCOMMON_FIFO_CHANNEL_ARM9, &tcommonFIFOCallback, NULL)) {
    	return 1; //Fatal Error
    }
	
    //Init filesystem
    if (!fatInitDefault()) {
    	#ifdef EFS
			if (!EFS_Init(EFS_AND_FAT|EFS_DEFAULT_DEVICE, NULL)) {
		#endif
		        libfatFail();
		        return 1;
		#ifdef EFS
			}
		#endif
    } else {
        chdir("/TouhouDS");
    }


    //Check install
    checkInstall();

    //Init default fonts
    createDefaultFontCache("font.ttf");
    defaultFontCache->AddFont("mono.ttf");

    //Init ASLib
	AS_Init(AS_MODE_MP3, 32*1024);
    AS_SetDefaultSettings(AS_ADPCM, 22050, 0);
    soundEnable();

    //Create logs
    initLog(0); //Default Size
    initCompilerLog(0); //Default Size

#ifdef ENABLE_WIFI
	initWifi();
    wifiConfig.Load("wifi.ini");
#endif

	fadeBlack(8);
    while (true) {
    	MainMenu2* menu = new MainMenu2();
    	menu->Run();
        CharInfo* ci = menu->GetSelectedCharacter();
        GameInfo* gi = menu->GetSelectedGame();
        s8 route = menu->GetSelectedRoute();
        s8 stage = menu->GetSelectedStage();
        u8 startLife = menu->startingLives;
        bool practice = menu->practice;
        bool remotePlay = menu->remotePlay;
        delete menu;

        if (!gi) { //Can't the start game without selecting a game
            gi = gameInfo[0];
        }
        if (!ci) { //Can't the start game without selecting a character
            ci = &charInfo[0];
        }
        if (route < 0) route = 0;

        //Run Game
    	fadeBlack(8);
        initSpellDecls();

    	Game* game = new Game(practice, remotePlay, startLife);
        game->Start(gi, ci, route, stage);
        if (!game->error) {
            game->SetHiScore(gi->GetHiScore(gi->GetRouteId(route), ci->GetId()));
            game->Run();
        }
        int error = game->error;
        delete game;

        if (remotePlay) {
			remote.Stop();
			stopWifi();
        }

        if (error) {
        	unfadeBlack2(8);
            scriptFail(error);
        	fadeBlack(8);
        }
    }

    destroyLog();
    destroyDefaultFontCache();
    return 0;
}
