#ifndef BOSS_H
#define BOSS_H

#include <string>

#include "chara_anim.h"
#include "sprite.h"

class LuaBossSpellcard {
	private:

	public:
		char* func;
		s32 power;
		u16 time;
		char* name;

		LuaBossSpellcard(const char* func, s32 power, u16, const char* name);
		LuaBossSpellcard(const LuaBossSpellcard& other);
		~LuaBossSpellcard();

		LuaBossSpellcard& operator=(const LuaBossSpellcard& other);

};

class Boss : public Sprite {
    private:
        DSSprite* nameSprites[4];
        DSSprite* spellcardNameSprites[4];
        DSSprite* bossIndicator;

        CharaAnimationSet animations;
        CharaAnimationType animationType;
        bool mirrorAnim;

        char* name;
        u8 numSpellcards;

        LuaMethodLink* luaSpellcard;
        SpellBook* spellcard;
        char* spellName;
        u16 time;

        std::list<LuaBossSpellcard> luaSpellcards;

        void OnSpellEnd(bool timeout=false);
        void DrawTextSprite(u16* screen, u16 x, u16 y, const char* string);
        void SetAnimation(CharaAnimationType type);

    public:
        s32 startPower;

        Boss(Game* game, Texture* texture=NULL,
            s32 x=0, s32 y=0, u16 w=0, u16 h=0, UV uv=UV(0, 0, 0, 0));
        virtual ~Boss();

        virtual void Destroy();
        virtual void Kill();
        virtual void UpdateSpellBook();
        virtual void Update();
        virtual void Draw();

        u16 GetTimeLeft();
        u8 GetSpellCardsLeft();

        void AddLuaSpellcard(const char* methodName, s32 pow, s32 t, const char* n);
        void SetName(const char* name);
        void SetSpellCard(SpellBook* spellcard, s32 life, u16 time, const char* name);
        void SetNumSpellcards(u8 sc);
        void SetCharaAnim(CharaAnimationType cat, SpellBook* anim);
        virtual void SetAnimationSpellBook(SpellBook* sb);
};

#endif
