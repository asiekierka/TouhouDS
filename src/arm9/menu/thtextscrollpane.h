#ifndef THTEXTSCROLLPANE_H
#define THTEXTSCROLLPANE_H

#include "../tcommon/gui/gui_types.h"
#include "../tcommon/gui/textscrollpane.h"

class THTextScrollPane : public TextScrollPane {
	private:

	public:
		THTextScrollPane(Screen* screen, FontCache* fc);
		virtual ~THTextScrollPane();

		virtual void DrawListItemForeground(s16 index, Rect bounds, bool selected);
};

#endif
