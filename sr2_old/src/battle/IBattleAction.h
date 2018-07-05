#ifndef SR_BATTLE_ACTION_H
#define SR_BATTLE_ACTION_H

namespace StoneRing{

    class IBattleAction
    {
    public:
        // Var is a slot for the script to store something. Such as, which item/spell was selected, etc
        virtual void Execute(ICharacter *pActor, ICharacter *pTarget, bool targetGroup, const SteelType &var)=0;
    private:
    };
}


#endif