#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

namespace StoneRing{

class ICharacter
{
public:
    virtual void modifyAttribute(const std::string &attribute, int add, float multiplier)=0;
    virtual int getMaxAttribute(const std::string &attribute) const = 0;
    virtual int getMinAttribute(const std::string &attribute) const = 0;
    virtual int getAttribute(const std::string &attribute) const = 0;
private:
};


class ICharacterGroup
{
public:
    virtual uint getCharacterCount() const = 0;
    virtual uint getSelectedCharacterIndex() const = 0;
    virtual uint getCasterCharacterIndex() const = 0;
    virtual ICharacter * getCharacter(uint index) const = 0;
    virtual ICharacter * getSelectedCharacter() const = 0;
    virtual ICharacter * getCasterCharacter() const = 0;
private:
};

};
#endif
