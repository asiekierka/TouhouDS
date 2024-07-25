#ifndef SPRITE_LIST_H
#define SPRITE_LIST_H

#include "thcommon.h"
#include "sprite.h"

class SpriteList {
	private:

    public:
        u32 size;
        u32 maxSize;
        Sprite** values;

        SpriteList(u32 size);
        ~SpriteList();
};

void SL_Append(SpriteList& list, Sprite* s);
void SL_Remove(SpriteList& list, u32 index);

void SL_Clear(SpriteList& list);
void SL_Compact(SpriteList& list);

inline
Sprite* SL_Next(SpriteList& list, u32& index) {
	#ifdef DEBUG
		if (index >= list.size) {
			printf("NEXT OUT OF BOUNDS\n");
			waitForAnyKey();
			return NULL;
		}
	#endif

	return list.values[index++];
}

inline
bool SL_Done(SpriteList& list, u32 index) {
	return index >= list.size;
}

#endif
