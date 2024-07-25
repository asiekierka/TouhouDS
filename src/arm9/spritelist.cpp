#include "spritelist.h"
#include "sprite.h"

SpriteList::SpriteList(u32 s) {
	values = new Sprite*[s];
	size = 0;
	maxSize = s;
}

SpriteList::~SpriteList() {
    delete[] values;
}

void SL_Clear(SpriteList& list) {
	for (u32 n = 0; n < list.size; n++) {
		if (list.values[n]) list.values[n]->listIndex = -1;
	}
	list.size = 0;
}

ITCM_CODE
void SL_Append(SpriteList& list, Sprite* s) {
	if (list.size >= list.maxSize) {
		SL_Compact(list);
		if (list.size >= list.maxSize) {
			iprintf("Moving past the end of a spritelist (size=%d, maxsize=%d) Infinite loop?\n", list.size, list.maxSize);
			waitForAnyKey();
		}
	}

	list.values[list.size] = s;
	s->listIndex = list.size;
	list.size++;
}

ITCM_CODE
void SL_Remove(SpriteList& list, u32 index) {
	if (list.values[index]) {
		list.values[index]->listIndex = -1;
		list.values[index] = NULL;
	}
}

ITCM_CODE
void SL_Compact(SpriteList& list) {
	Sprite** read = list.values;
	Sprite** write = read;
	int blanks = 0;

	for (u32 n = 0; n < list.size; n++) {
		if (*read) {
			*write = *read;
			(*write)->listIndex = n - blanks;
			write++;
		} else {
			blanks++;
		}
		read++;
	}

	list.size -= blanks;
}
