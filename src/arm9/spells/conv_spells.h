#ifndef CONV_SPELLS_H
#define CONV_SPELLS_H

class SaySpell : public Spell {
    private:
    	Param *slotP, *nameP, *textP;

    	u8 slot;
        const char* name;
        const char* text;
        bool waitingForInput;

    public:
    	SPELL_STD(SaySpell)
        virtual void Reset();
};

class ConvImageSpell : public Spell {
	private:
		Param *slotP, *imageP;

		u8 slot;
		const char* image;

	public:
    	SPELL_STD_CONSTR(ConvImageSpell)

    	virtual void Reset();
    	virtual void Update();

    	static void CreateDecls(SpellDecl** decls, int& declsL);
};

#endif
