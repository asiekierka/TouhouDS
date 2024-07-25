#include "scripterrorscreen.h"
#include "mainmenu2.h"
#include "thtextscrollpane.h"
#include "../tcommon/gui/gui_common.h"

using namespace std;

ScriptErrorScreen::ScriptErrorScreen(GUI* gui, u16* textureI) : Screen(gui) {
	SetTexture(textureI);
	SetBackgroundImage(textureI);

	textPane = new THTextScrollPane(this, defaultFontCache);
	textPane->SetTextWrapping(true);
	textPane->SetTextBufferSize(64 * 1024);
	textPane->SetDefaultMargins(8, 8, 0, 0);
	textPane->SetItemHeight(16);
	textPane->SetFont("font.ttf");
	textPane->SetFontSize(10, false);
	textPane->SetBounds(0, 0, 256, 192);
	textPane->SetShowSelections(false);
}

ScriptErrorScreen::~ScriptErrorScreen() {

}

void ScriptErrorScreen::SetError(int code) {
	char temp[256];
    sprintf(temp, "Script Parse Error\nError code: %6d\n\n", code);
    textPane->AppendText(temp);

    if (compilerLogBuffer) {
    	vector<LogEntry> entries = compilerLogBuffer->GetEntries();
    	for (u32 n = 0; n < entries.size(); n++) {
    		textPane->AppendText(entries[n].message);
    	}
    }
}
