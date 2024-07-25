#ifndef DOWNLOADSCREEN_H
#define DOWNLOADSCREEN_H

#include "mainmenu2.h"
#include "thtextscrollpane.h"
#include "../parser/repository.h"
#include "../tcommon/gui/screen.h"
#include "../tcommon/gui/button.h"

#include <string>

class DownloadScrollPane : public THTextScrollPane {
	private:
        Repository* repo;
        std::vector<std::string> markedForInstall;

        void ToggleMark(int s);

	public:
		DownloadScrollPane(Screen* screen, FontCache* fc);
		virtual ~DownloadScrollPane();

		virtual bool OnTouch(u32& down, u32& held, touchPosition touch);
		virtual void Update(u32& down, u32& held, touchPosition touch);
		virtual void DrawListItemForeground(s16 index, Rect bounds, bool selected);
		void Install(const char* path);

		bool IsMarkedForInstall(const char* id);

		void SetRepository(Repository* repo);
};

class DownloadScreen : public Screen, public ButtonListener {
	private:
		MainMenu2* menu;
		Button* applyButton;
		Button* gameButton;
		Button* charaButton;
		Button* wifiButton;
		Button* backButton;
		DownloadScrollPane* scrollPane;

		s8 mode;
		Repository gameRepo;
		Repository charaRepo;

	public:
		DownloadScreen(GUI* gui, MainMenu2* menu);
		virtual ~DownloadScreen();

		virtual void Update(u32& keysDown, u32& keysHeld, touchPosition touch);
		virtual void OnButtonPressed(Button* button);
		virtual void DrawForeground();
		virtual void Activate();
		virtual void Deactivate();
		virtual void Cancel();
		void UpdateRepositories();

};

#endif
