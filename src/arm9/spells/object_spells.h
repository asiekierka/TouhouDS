#ifndef OBJECT_SPELLS_H
#define OBJECT_SPELLS_H

class ObjectImageSpell : public Spell {
    private:
        Game* game;
        Param *textureP, *xP, *yP, *wP, *hP;

        const Texture* texture;
        u8 x, y, w, h;

    public:
    	SPELL_STD(ObjectImageSpell)
        virtual void Reset();
};

class AdvanceSpell : public Spell {
    private:
        Param *amountP;

        s32 amount; //In 20.12 format

    public:
    	SPELL_STD(AdvanceSpell)
        virtual void Reset();
};

class AdvanceSideSpell : public Spell {
    private:
        Param *amountP;

        s32 amount; //In 20.12 format

    public:
    	SPELL_STD(AdvanceSideSpell)
        virtual void Reset();
};

class DieSpell : public Spell {
    private:

    public:
        SPELL_STD(DieSpell)
};

class DropSpell : public Spell {
    private:
        Param *itemP, *amountP, *chanceP;

        ItemType item;
        u8 amount;
        u8 chance;

    public:
    	SPELL_STD(DropSpell)
        virtual void Reset();
};

class MoveSpell : public Spell {
    private:
        Param *xP, *yP;

        s32 x, y;
        bool relative;

    public:
    	MoveSpell(Game* game, Param** p, int pL, bool relative=false);
        virtual ~MoveSpell();

        virtual void Reset();
    	virtual void Update();
    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

class MoveToSpell : public Spell {
    private:
        Param *xP, *yP, *sP;

        s32 x, y, s;
        s32 angle; //Calculated

    public:
    	SPELL_STD(MoveToSpell)
        virtual void Reset();

};

class SpeedToSpell : public Spell {
    private:
        Param *valueP, *accelP;

        s32 value;
        s32 accel;

    public:
    	SPELL_STD(SpeedToSpell)
        virtual void Reset();
};

class TurnToSpell : public Spell {
    private:
        Param *angleP, *deltaP;

        AngleTarget target;
        s32 value; //Angle
        s32 delta;

    public:
    	SPELL_STD(TurnToSpell)
        virtual void Reset();
};

class HitBoxSpell : public Spell {
    private:
        Param* rP;

        s32 r;

    public:
    	SPELL_STD(HitBoxSpell)
        virtual void Reset();
};

class PowerSpell : public Spell {
    private:
        Param* powerP;

        s32 power;

    public:
    	SPELL_STD(PowerSpell)
        virtual void Reset();
};

class SpeedSpell : public Spell {
    private:
        Param* valueP;

        s32 value; //Speed in 20.12 fixed
        bool isAccel;

    public:
        SpeedSpell(Game* g, Param** p, int pL, bool isAccel=false);
        virtual ~SpeedSpell();

        virtual void Reset();
    	virtual void Update();
    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

class AngleSpell : public Spell {
    private:
        Param* angleP;

        AngleTarget target;
        s32 value; //Angle
        bool isAccel;
        bool relative;

    public:
        AngleSpell(Param* angle, bool isAccel=false, bool relative=false);
        virtual ~AngleSpell();

        virtual void Reset();
    	virtual void Update();
    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

class VisibilitySpell : public Spell {
    private:
        bool visibility;

    public:
        VisibilitySpell(bool visibility);
        virtual ~VisibilitySpell();

        virtual void Update();
    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

class SizeSpell : public Spell {
	private:
		Param *wP, *hP;

		s16 w, h;

	public:
    	SPELL_STD(SizeSpell)
        virtual void Reset();

};

class DrawAngleSpell : public Spell {
	private:
		Param *modeP, *valueP;

		DrawAngleMode mode;
        AngleTarget target;
        s32 value; //Angle

	public:
		SPELL_STD(DrawAngleSpell)
		virtual void Reset();

};

#endif
