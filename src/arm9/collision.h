#ifndef COLLISION_H
#define COLLISION_H

#include "thcommon.h"

#define COL_GRID_SIZE 16
#define COL_PAD		  4
#define COL_GRID_W    (((LEVEL_WIDTH  + COL_GRID_SIZE-1) / COL_GRID_SIZE) + 2*COL_PAD)
#define COL_GRID_H    (((LEVEL_HEIGHT + COL_GRID_SIZE-1) / COL_GRID_SIZE) + 2*COL_PAD)

#if COL_GRID_SIZE == 16
#define f32tocol(x) (((x)>>16) + COL_PAD)
#else
#warning "Using unoptimized f32tocol() instance"
#define f32tocol(x) (f32toint(x) / COL_GRID_SIZE + COL_PAD)
#endif

enum ColNodeType {
	COL_default,
	COL_circle,
	COL_segment
};

class ColNode {
	public:
		Sprite* sprite;
		LL_Node<ColNode> listNode;

		ColNodeType type;
		u8 id;
		s8 gx, gy;

		ColNode(Sprite* s, ColNodeType t, u8 id);
		virtual ~ColNode();

		virtual int GetIntRadius() = 0;
		virtual void GetBounds(Rect* out) = 0;
};

class CircleColNode : public ColNode {
	public:
		s32 rad;

		CircleColNode(Sprite* s, u8 id);
		virtual ~CircleColNode();

		virtual int GetIntRadius();
		virtual void GetBounds(Rect* out);

		void SetRadius(s32 r);
};

class SegmentColNode : public ColNode {
	private:
		s32 length;
		s16 unitX, unitY;

	public:
		s32 baseDX, baseDY;
		s32 dx, dy;
		s32 thickness;
		bool autoRotate;

		SegmentColNode(Sprite* s, u8 id);
		virtual ~SegmentColNode();

		void GetUnitXY(s16* ux, s16* uy);
		s32 GetLength();
		virtual int GetIntRadius();
		virtual void GetBounds(Rect* out);

		void Set(bool autoRotate, s32 dx, s32 dy, s32 thickness);
		void SetRotation(s32 da);
};

class ColGrid {
	private:

	public:
		THLinkedList<ColNode> cells[COL_GRID_W][COL_GRID_H][NUM_SPRITE_TYPES];
		THLinkedList<ColNode> largeObjects[NUM_SPRITE_TYPES];
		bool colMatrix[NUM_SPRITE_TYPES][NUM_SPRITE_TYPES];

		ColGrid();
		~ColGrid();
};

void autoCollectItems(Player* player, SpriteList* items);
void onHit(Sprite* s1, int idx1, Sprite* s2, int idx2, s32 s2p);
void onHit(ColNode* n1, int idx1, ColNode* n2, int idx2, s32 s2p);
void collide(ColNode* n1, ColNode* n2);
void collide(SpriteList** sprites, ColGrid* colGrid, bool autoCollect, bool playerInvincible);
void updateColNodes(Sprite* sprite, ColGrid* colGrid);

bool col_circle_segment(CircleColNode* c, SegmentColNode* s);
bool col_segment_segment(SegmentColNode* s1, SegmentColNode* s2);

#endif
