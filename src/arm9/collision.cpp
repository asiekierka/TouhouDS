#include "collision.h"

#include <vector>
#include "item.h"
#include "player.h"
#include "spritelist.h"
#include "lua/thlua.h"

#define IS_SHOT_TYPE(x) \
	(x->type == SPRITE_playerShot || x->type == SPRITE_enemyShot)

using namespace std;

//-----------------------------------------------------------------------------

ColGrid::ColGrid() {
	for (int a = 0; a < NUM_SPRITE_TYPES; a++) {
		for (int b = 0; b < NUM_SPRITE_TYPES; b++) {
			colMatrix[a][b] = false;
		}
	}

	colMatrix[SPRITE_player][SPRITE_enemy] = colMatrix[SPRITE_enemy][SPRITE_player] = true;
	colMatrix[SPRITE_player][SPRITE_enemyShot] = colMatrix[SPRITE_enemyShot][SPRITE_player] = true;
	colMatrix[SPRITE_playerShot][SPRITE_enemy] = colMatrix[SPRITE_enemy][SPRITE_playerShot] = true;
	colMatrix[SPRITE_remote][SPRITE_enemy] = colMatrix[SPRITE_enemy][SPRITE_remote] = true;
}
ColGrid::~ColGrid() {

}

//-----------------------------------------------------------------------------

ColNode::ColNode(Sprite* s, ColNodeType t, u8 cid)
:	sprite(s), type(t), id(cid), gx(0), gy(0)
{
	listNode.value = this;
}

ColNode::~ColNode() {
	listNode.Remove();
}

//-----------------------------------------------------------------------------

CircleColNode::CircleColNode(Sprite* s, u8 id)
:	ColNode(s, COL_circle, id), rad(0)
{

}

CircleColNode::~CircleColNode() {

}

void CircleColNode::SetRadius(s32 r) {
	if (rad != r) {
		rad = r;
		listNode.Remove();
	}
}

int CircleColNode::GetIntRadius() {
	return f32toint(rad);
}

void CircleColNode::GetBounds(Rect* out) {
	*out = Rect(sprite->x - rad, sprite->y - rad, rad<<1, rad<<1);
}

//-----------------------------------------------------------------------------

SegmentColNode::SegmentColNode(Sprite* s, u8 id)
:	ColNode(s, COL_segment, id), length(0), unitX(0), unitY(0),
	baseDX(0), baseDY(0), dx(0), dy(0), thickness(0), autoRotate(false)
{

}

SegmentColNode::~SegmentColNode() {

}

void SegmentColNode::GetUnitXY(s16* ux, s16* uy) {
	GetLength(); //Forces initialization of unitXY

	*ux = unitX;
	*uy = unitY;
}

s32 SegmentColNode::GetLength() {
	if (length == 0 && (dx != 0 || dy != 0)) {
		length = sqrtf32(mulf32(dx, dx) + mulf32(dy, dy));
		unitX = divf32(dx, length);
		unitY = divf32(dy, length);
	}

	return length;
}

int SegmentColNode::GetIntRadius() {
	return f32toint(MAX(dx, dy));
}

void SegmentColNode::GetBounds(Rect* out) {
	*out = Rect(sprite->x, sprite->y, dx, dy);
}

void SegmentColNode::Set(bool autorot, s32 x, s32 y, s32 t) {
	unitX = unitY = 0;
	length = 0;

	autoRotate = autorot;
	baseDX = dx = x;
	baseDY = dy = y;
	thickness = t;
}

void SegmentColNode::SetRotation(s32 da) {
	unitX = unitY = 0;
	length = 0;

	s32 sinA = fastSin(da);
	s32 cosA = fastCos(da);
	dx = mulf32(baseDX, cosA) - mulf32(baseDY, sinA);
	dy = mulf32(baseDX, sinA) + mulf32(baseDY, cosA);

	//iprintf("%03d: %03d %03d\n", da>>6, dx>>12, dy>>12);
}

//-----------------------------------------------------------------------------

void onHit(Sprite* s1, int col1Id, Sprite* s2, int col2Id, s32 s2p) {
	SpriteType s1t = s1->type;
	SpriteType s2t = s2->type;

	if (s1t == SPRITE_item) {
		((Item*)s1)->Collect();
	} else if (s2t == SPRITE_item) {
		if (s1t == SPRITE_player) {
			((Player*)s1)->AddItem(((Item*)s2)->itemType);
		}
	} else {
        if (!s1->IsInvincible() && s2p > 0) {
            s1->power -= s2p;
            if (s1->power <= 0 || IS_SHOT_TYPE(s1)) {
                s1->Destroy();
            } else {
            	s1->damageFlash = DAMAGE_FLASH_DEFAULT;
            }
        }
	}

	if (s1->luaLink) {
		s1->luaLink->Call("onCollision", "dfffdd", s2t, s2p, s2->x, s2->y, col1Id, col2Id);
	}
}
void onHit(ColNode* n1, ColNode* n2, s32 s2p) {
	onHit(n1->sprite, n1->id, n2->sprite, n2->id, s2p);
}

ITCM_CODE
void collide(ColNode* n1, ColNode* n2) {
	Sprite* s1 = n1->sprite;
	Sprite* s2 = n2->sprite;
    if (!s1->drawData.visible || !s2->drawData.visible) {
        return;
    }

	//collide function assumes s1type <= s2type, swap if not the case
	if (s2->type < s1->type) {
		Sprite*  tempS = s1;
		ColNode* tempN = n1;

		s1 = s2;
		n1 = n2;
		s2 = tempS;
		n2 = tempN;
	}

    int s1t = s1->type;
    int s1p = s1->power;

    int s2t = s2->type;
    int s2p = s2->power;

    s32 dx = abs(s2->x - s1->x);
    s32 dy = abs(s2->y - s1->y);

    if (s1t == SPRITE_player) {
    	s1p = 0;
    	if (!s2->grazed) {
    		Player* p = ((Player*)s1);
    		if (dx < p->grazeRange && dy < p->grazeRange) {
    			p->Graze(s2);
    		}
    	}
    }
   	if (s2t == SPRITE_item) {
   		s2p = 0;
   	}

	bool hit = false;
	if (n1->type == COL_circle && n2->type == COL_circle) {
		CircleColNode* cn1 = (CircleColNode*)n1;
		CircleColNode* cn2 = (CircleColNode*)n2;
		s32 r = cn1->rad + cn2->rad;
		hit = (mulf32(dx, dx) + mulf32(dy, dy) < mulf32(r, r));
	} else if (n1->type == COL_circle && n2->type == COL_segment) {
		hit = col_circle_segment((CircleColNode*)n1, (SegmentColNode*)n2);
	} else if (n1->type == COL_segment && n2->type == COL_circle) {
		hit = col_circle_segment((CircleColNode*)n2, (SegmentColNode*)n1);
	} else if (n1->type == COL_segment && n2->type == COL_segment) {
		hit = col_segment_segment((SegmentColNode*)n2, (SegmentColNode*)n1);
	} else {
		iprintf("unimplemented collision mode: %dx%d\n", n1->type, n2->type);
	}

	if (hit) {
		onHit(n1, n2, s2p);
		onHit(n2, n1, s1p);
	}
}

bool canStillCollide(Sprite* s, int s2t) {
	if (s->listIndex < 0) return false;
	if (s->type == SPRITE_player) {
		return !s->IsInvincible() || s2t <= SPRITE_item;
	} else {
		return !s->IsInvincible();
	}
}

void collide(SpriteList** sprites, ColGrid* colGrid, bool autoCollect,
		bool playerInvincible)
{
	//int colTests = 0;
	bool (*colMatrix)[NUM_SPRITE_TYPES] = colGrid->colMatrix;
    for (int t1 = 0; t1 < NUM_SPRITE_TYPES; t1++) {
    	bool possibleSSCollisions = false; //Same Size
    	bool possibleLSCollisions = false; //Large Small
    	for (int t2 = 0; t2 < NUM_SPRITE_TYPES; t2++) {
    		if (colMatrix[t1][t2]) {
    			possibleSSCollisions = true;
    		}
    		if (colMatrix[t2][t1]) {
    			possibleLSCollisions = true;
    		}
    	}
    	if (!possibleSSCollisions && !possibleLSCollisions) {
    		continue;
    	}

		LL_Node<ColNode> *node, *nodeNext;

		u32 index = 0;
        while (!SL_Done(*sprites[t1], index)) {
        	Sprite* s = SL_Next(*sprites[t1], index);
        	if (!s) continue;

        	for (u32 col1Idx = 0; col1Idx < s->colNodes.size(); col1Idx++) {
        		ColNode* scol = s->colNodes[col1Idx];

        		int rad1 = scol->GetIntRadius();
				int amin, amax, bmin, bmax, t2min;
				if (t1 == SPRITE_player || t1 == SPRITE_item || rad1 > COL_GRID_SIZE) {
					//Large <-> Large Collisions
					if (possibleSSCollisions) {
						THLinkedList<ColNode>* largeObjects = colGrid->largeObjects;
						for (int t2 = t1+1; t2 <= NUM_SPRITE_TYPES; t2++) {
							if (!colMatrix[t1][t2]) continue;

							LL_Node<ColNode>* node = largeObjects[t2].first;
							while (node && canStillCollide(s, t2)) {
								LL_Node<ColNode>* nodeNext = node->next;
								collide(scol, node->value);
								if (s->listIndex < 0) {
									goto loopend;
								}
								node = nodeNext;
							}
						}
					}

					if (!possibleLSCollisions) {
						if (t1 == SPRITE_player) {
							iprintf("%d\n", t1);
						}
						continue;
					}

					s32 rad32 = inttof32(rad1);
					if (t1 == SPRITE_player) {
						rad32 = ((Player*)s)->grazeRange;
					} else if (t1 == SPRITE_item) {
						rad32 = ITEM_MAGNET_RAD;
					}
					amin = f32tocol(s->x-rad32) - 1;
					amax = f32tocol(s->x+rad32) + 1;
					bmin = f32tocol(s->y-rad32) - 1;
					bmax = f32tocol(s->y+rad32) + 1;
					t2min = 0;
				} else {
					if (!possibleSSCollisions) {
						continue;
					}

					amin = scol->gx - 1;
					amax = scol->gx + 1;
					bmin = scol->gy - 1;
					bmax = scol->gy + 1;
					t2min = t1+1;
				}

				if (amin < 0)   		 amin = 0;
				if (amax > COL_GRID_W-1) amax = COL_GRID_W-1;
				if (bmin < 0) 			 bmin = 0;
				if (bmax > COL_GRID_H-1) bmax = COL_GRID_H-1;

				//Large/Small <-> Small Collisions
				for (int t2 = t2min; t2 < NUM_SPRITE_TYPES; t2++) {
					if (!colMatrix[t1][t2]) continue;

					for (int a = amin; a <= amax; a++) {
						for (int b = bmin; b <= bmax; b++) {
							//colTests++;

							node = colGrid->cells[a][b][t2].first;
							while (node && canStillCollide(s, t2)) {
								nodeNext = node->next;
								collide(scol, node->value);
								if (s->listIndex < 0) {
									goto loopend;
								}
								node = nodeNext;
							}
						}
					}
				}
        	}

			loopend:
				//Do nothing
			;
		}
	}
    //iprintf("%d\n", colTests);
}

ITCM_CODE
void updateColNodes(Sprite* sprite, ColGrid* colGrid) {
    int nsx = f32tocol(sprite->x);
    int nsy = f32tocol(sprite->y);

    vector<ColNode*>::const_iterator itr = sprite->colNodes.begin();
    vector<ColNode*>::const_iterator end = sprite->colNodes.end();
    while (itr != end) {
    	ColNode* scol = *itr;
    	++itr;

        int rad = scol->GetIntRadius();
    	if (rad > COL_GRID_SIZE) {
    		scol->gx = nsx;
    		scol->gy = nsy;
    		if (!scol->listNode.list) {
    			colGrid->largeObjects[sprite->type].Append(&scol->listNode);
    		}
    	} else {
    		if (nsx < 0 || nsx >= COL_GRID_W || nsy < 0 || nsy >= COL_GRID_H) {
    			if (sprite->dieOutsideBounds) {
    				sprite->Kill();
    			} else {
    				scol->listNode.Remove();
    			}
    		} else {
    			if (nsx != scol->gx || nsy != scol->gy) {
    				scol->gx = nsx;
    				scol->gy = nsy;
    				scol->listNode.Remove();
    				colGrid->cells[nsx][nsy][sprite->type].Append(&scol->listNode);
    			}
    		}
    	}
    }
}

void autoCollectItems(Player* player, SpriteList* items) {
    if (!player || player->listIndex < 0) {
    	return;
    }

	s32 x = player->x;
	s32 y = player->y;
	s32 maxR = 0;
	for (u32 n = 0; n < player->colNodes.size(); n++) {
		maxR = MAX(maxR, player->colNodes[n]->GetIntRadius());
	}
	s32 rs1 = inttof32(maxR * maxR);
	maxR = inttof32(maxR);

	bool autoCollect = (y <= ITEM_GET_LINE);

	Item* item;
	u32 index = 0;
	while (!SL_Done(*items, index)) {
		item = (Item*)SL_Next(*items, index);
		if (!item) continue;

		s32 dx = (item->x - x);
		s32 dy = (item->y - y);

		if (autoCollect || item->itemType == IT_magnetPoint) {
			item->x += (dx > 0 ? -ITEM_AC_ATR : ITEM_AC_ATR             );
			item->y += (dy > 0 ? -ITEM_AC_ATR : ITEM_AC_ATR - ITEM_SPEED);
			if (mulf32(dx, dx) + mulf32(dy, dy) < rs1 + ITEM_AC_ATR*3) {
				onHit(player, -1, item,   -1, 0);
				onHit(item,   -1, player, -1, 0);
			}
		} else {
			if (mulf32(dx, dx) + mulf32(dy, dy) < ITEM_MAGNET_RAD_SQ) {
				item->x += (dx > 0 ? -ITEM_ATR : ITEM_ATR             );
				item->y += (dy > 0 ? -ITEM_ATR : ITEM_ATR - ITEM_SPEED);
				if (abs(item->x - x) <= ITEM_ATR + maxR &&
					abs(item->y - y) <= ITEM_ATR + maxR)
				{
					onHit(player, -1, item,   -1, 0);
					onHit(item,   -1, player, -1, 0);
				}
			}
		}
	}
}

void closestPointOnLineSegment(s32* outX, s32* outY, SegmentColNode* s, s32 sx, s32 sy) {
	s32 x0 = s->sprite->x;
	s32 y0 = s->sprite->y;
	s32 cx = sx - x0;
	s32 cy = sy - y0;

	s16 ux, uy;
	s->GetUnitXY(&ux, &uy);

	s32 t = mulf32(cx, ux) + mulf32(cy, uy);
	if (t <= 0) {
		*outX = x0;
		*outY = y0;
	} else if (t >= s->GetLength()) {
		*outX = x0 + s->dx;
		*outY = y0 + s->dy;
	} else {
		*outX = x0 + mulf32(ux, t);
		*outY = y0 + mulf32(uy, t);
	}
}

bool col_circle_segment(CircleColNode* c, SegmentColNode* s) {
	s32 cx = c->sprite->x;
	s32 cy = c->sprite->y;

	s32 colX, colY;
	closestPointOnLineSegment(&colX, &colY, s, cx, cy);

	s32 dx = cx-colX;
	s32 dy = cy-colY;
	s32 r = c->rad + s->thickness;
	return (mulf32(dx, dx) + mulf32(dy, dy) <= mulf32(r, r));
}

bool col_segment_segment(SegmentColNode* s1, SegmentColNode* s2) {
	s32 ptX[4];
	s32 ptY[4];

	ptX[0] = s1->sprite->x;
	ptY[0] = s1->sprite->y;
	ptX[1] = ptX[0] + s1->dx;
	ptY[1] = ptY[0] + s1->dy;

	ptX[2] = s2->sprite->x;
	ptY[2] = s2->sprite->y;
	ptX[3] = ptX[2] + s2->dx;
	ptY[3] = ptY[2] + s2->dy;

	closestPointOnLineSegment(&ptX[0], &ptY[0], s2, ptX[0], ptY[0]);
	closestPointOnLineSegment(&ptX[1], &ptY[1], s2, ptX[1], ptY[1]);
	closestPointOnLineSegment(&ptX[2], &ptY[2], s1, ptX[2], ptY[2]);
	closestPointOnLineSegment(&ptX[3], &ptY[3], s1, ptX[3], ptY[3]);

	s32 minDistSq = inttof32(999999);
	for (int n = 0; n < 4; n++) {
		s32 d = mulf32(ptX[n], ptX[n]) + mulf32(ptY[n], ptY[n]);
		if (d < minDistSq) {
			minDistSq = d;
		}
	}

	s32 r = s1->thickness + s2->thickness;
	return (minDistSq <= mulf32(r, r));
}
