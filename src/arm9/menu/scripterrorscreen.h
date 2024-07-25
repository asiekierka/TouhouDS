#ifndef SCRIPTERRORSCREEN_H
#define SCRIPTERRORSCREEN_H

#include "../tcommon/gui/screen.h"

class ScriptErrorScreen : public Screen {
	private:
		TextScrollPane* textPane;

	public:
		ScriptErrorScreen(GUI* gui, u16* textureI);
		virtual ~ScriptErrorScreen();

		void SetError(int code);
};

#endif
