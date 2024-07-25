#ifndef MAINMENU2_H
#define MAINMENU2_H

#include "../thcommon.h"
#include "../sound.h"
#include "../tcommon/gui/gui_types.h"

class MainMenu2 {
	private:
		GUI* gui;
		bool quit;

	public:
		u16* textureI;
		u16* titleTextureI;
		u16* backgroundI;
		u16* titleI;
		SoundManager soundManager;

		s16  selectedCharacter;
		s16  selectedGame;
		s8   selectedRoute;
		s8   selectedStage;
		bool practice;
		bool remotePlay;
		u8   startingLives;

		MainMenu2();
		virtual ~MainMenu2();

		void Run();
		void Quit();
		void LoadTitleImages();
		void UnloadTitleImages();

		CharInfo* GetSelectedCharacter();
		GameInfo* GetSelectedGame();
		s8 GetSelectedRoute();
		s8 GetSelectedStage();

};

#endif
