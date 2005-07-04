#ifndef SR_ITEM_H
#define SR_ITEM_H

#include <string>


namespace StoneRing{



class Item
{
 public:
  Item();
  ~Item();

  enum eItemType { ITEM, WEAPON, ARMOR, RUNE, SPECIAL, SYSTEM };

  void setItemType ( eItemType type );
  eItemType getItemType() const;

  std::string getName() const;
  void setName(const std::string &name );

  static std::string ItemTypeAsString ( Item::eItemType type );

 private:

  std::string mItemName;
  eItemType meItemType;
};

bool   operator < ( const Item &lhs, const Item &rhs );



};




#endif
