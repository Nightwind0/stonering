#ifndef SR_EFFECT_H
#define SR_EFFECT_H

namespace StoneRing {

    class Effect
    {
    public:
	Effect();
	virtual ~Effect();

	enum eType { WEAPON_DAMAGE, MAGIC_DAMAGE, STATUS_EFFECT, ANIMATION, ATTRIBUTE_EFFECT , ATTACK};

	virtual eType getEffectType() const = 0;
    private:
    };

}

#endif
