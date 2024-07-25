#ifndef SPRITE_H
#define SPRITE_H

#include "thcommon.h"
#include "collision.h"

class DrawData {
	private:

	public:
		const Texture* texture;
		UV uv;
		s32 drawAngle;
		s32 sinA, cosA;
		v16 vw, vh;
		bool visible;
		bool clip; //Don't draw the sprite when it's outside the current screen

		DrawData(Texture* tex, UV duv, v16 w, v16 h, s32 da);
		~DrawData();

		void OnSizeChanged();
		void SetDrawAngle(s32 a);
};

class Sprite {
    private:
        s32 speed;
        s32 speedInc;
        s32 angle;
        s32 angleInc;

        s32 speedX, speedY;
        bool speedVecDirty;

        void DeleteSpells();

    protected:
        SpellBook* initSpellBook;
        SpellBook* lifeSpellBook;
        SpellBook* deathSpellBook;
        SpellBook* animationSpellBook;
        DrawAngleMode drawAngleMode;

    public:
        Game* game;
        s32 x, y;
        s16 listIndex;
        SpriteType type;
        s8  z;

        s32 power;
        u16 invincible;
        s16 refcount;
        u8  damageFlash;
        bool grazed;
        bool dieOutsideBounds;
        bool wasOnScreen; //If the sprite was ever visible, only updated if dieOutsideBounds is true

        DrawData drawData;
        LuaObjectLink* luaLink;
        LuaLink* luaAnimatorLink;
        std::vector<ColNode*> colNodes;

        Sprite(Game* game, SpriteType type, Texture* texture=NULL,
        		s32 x=-50, s32 y=-50, u16 w=0, u16 h=0, UV uv=UV(0, 0, 0, 0));
        virtual ~Sprite();

        virtual void Destroy(); //Killed in a violent way
        virtual void Kill();    //Remove Sprite command
        virtual void UpdateSpellBook();
        virtual void Update();
        virtual void Draw();

        virtual bool IsInvincible();
        s32 GetSpeed() { return speed; }
        s32 GetSpeedInc() { return speedInc; }
        s32 GetAngle() { return angle; }
        s32 GetAngleInc() { return angleInc; }
        DrawAngleMode GetDrawAngleMode() { return drawAngleMode; }
        s32 GetDrawAngle() { return drawData.drawAngle; }

        void Advance(s32 distance);
        void AdvanceSide(s32 distance);
        void SetAngle(s32 angle);
        void SetAngleInc(s32 da);
        void SetSpeed(s32 speed);
        void SetSpeedInc(s32 ds);
        void SetDrawAngle(s32 da);
        void SetHitRadius(s32 rad);
        void SetColCircle(u8 id, s32 rad);
        void SetColSegment(u8 id, bool autoRotate, s32 dx, s32 dy, s32 thickness);
        void RemoveColNode(u8 id);

        virtual void SetDrawAngleMode(DrawAngleMode dam);
        virtual void SetInitSpellBook(SpellBook* s);
        virtual void SetLifeSpellBook(SpellBook* s);
        virtual void SetDeathSpellBook(SpellBook* s);
        virtual void SetAnimationSpellBook(SpellBook* s);
};

#endif
