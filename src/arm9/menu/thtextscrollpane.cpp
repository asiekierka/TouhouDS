#include "thtextscrollpane.h"
#include "../tcommon/gui/gui_common.h"

THTextScrollPane::THTextScrollPane(Screen* screen, FontCache* fc)
: TextScrollPane(screen, Rect(0,0,16,16), Rect(0,16,16,16), Rect(0,32,16,16), fc)
{

}
THTextScrollPane::~THTextScrollPane() {

}

void THTextScrollPane::DrawListItemForeground(s16 index, Rect r, bool selected) {
	TextScrollPane::DrawListItemForeground(index, r, selected);

	if (selected && showSelections) {
		const int vw = VERTEX_SCALE(16);
		const int vh = VERTEX_SCALE(listItemH);

		//drawQuad(texture, 0, inttof32(r.y), vs<<1, vs, Rect(0, 80, 32, 16));
		drawQuad(texture, inttof32(r.x+32), inttof32(r.y), vw, vh, Rect(32, 80, 16, 16));
		drawQuad(texture, inttof32(r.x+r.w-16), inttof32(r.y), vw, vh, Rect(48, 80, 16, 16));
	}
}
