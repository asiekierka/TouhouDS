#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include "parser/compiler.h"

class Player : public Sprite {
    private:
        bool focus;
        s16 cooldown;
        s16 bombCooldown;
        FocusSprite* focusSprite;

        SpellBookTemplate* fireSpells[8];
        SpellBook* fireSpellBooks[8];

        Param** fireSpellParams;
        u8 fireSpellParamsL;
        BombInfo* bomb;
        BombInfo* bombFocus;

        s32 shotPowerMul;
        s32 shotPowerMul1, shotPowerMul2;
        s32 speed;
        s32 speedFocus;
        u32 points;
        u16 invincible;
        u16 shotPower;
        u16 extraPoints;
        u16 graze;
        u8  lives;
		u8  startBombs;
        u8  bombs;
        s8  dying;

        s8   sndShot;
        bool justGrazed;

        s32 lastX;
        int animation;
        int animationDir;
        int animationFrame;

        char fireFuncName[128];

        void UpdateDying();
        void UpdateAnimation();
        void ChangeShotPower(s16 delta);

    public:
        const u8 playerId;
        s32 grazeRange;

        LuaLink* luaFireScript;

        Player(Game* game, u8 pid, Texture* texture, const char* charaId);
        virtual ~Player();

        virtual void Destroy();
        virtual void AddItem(ItemType itemType);
        virtual Bomb* CreateBomb();
        virtual void Update();
        virtual void Draw();

        void Graze(Sprite* s);

        bool HasFullPower();
        virtual bool IsInvincible();
        virtual u8 GetKeys();

        bool IsFocus();
        u32 GetAttackLevel();
        s32 GetAttackPower1();
        s32 GetAttackPower2();
        u32 GetPoints();
        u16 GetShotPower();
        u16 GetExtraPoints();
        u16 GetGraze();
        u8  GetLives();
        u8  GetBombs();

        void SetPoints(u32 points);
        void SetShotPower(u16 shotPower);
        void SetExtraPoints(u16 extraPoints);
        void SetGraze(u16 graze);
        void SetLives(u8 lives);
        void SetBombs(u8 bombs);

        void SetShotPowerMul(s32 s);
        void SetSpeed(s32 s);
        void SetSpeedFocus(s32 sf);
        void SetGrazeRange(s32 r);
        void SetStartBombs(u8 b);

        void SetBomb(BombInfo* bomb);
        void SetBombFocus(BombInfo* bombFocus);
        void SetBombCooldown(u16 frames);
};

#endif
