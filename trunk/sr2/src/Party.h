

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include "Level.h"

typedef unsigned int uint;


namespace StoneRing{

class Party
{
 public:


  bool getGold() const;
  bool hasItem(Item::eItemType type, const std::string &item) const;
  bool hasItem(ItemRef *pItemRef );
  bool didEvent(const std::string &event) const;
  uint getLevelX() const;
  uint getLevelY() const;

  static Party * getInstance();

 private:

  static Party * mInstance;
  Party();
  ~Party();
};


};


#endif
