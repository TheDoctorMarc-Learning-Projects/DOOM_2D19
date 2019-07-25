#ifndef _J1ENTITY_LOOT_COIN_H__
#define _J1ENTITY_LOOT_COIN_H__

#include "j1EntityLoot.h"

class j1EntityLootCoin : public j1EntityLoot
{

public:
	j1EntityLootCoin(float posX, float posY, LOOT_TYPE loot_type, std::string name, bool classicFigure);
	~j1EntityLootCoin() {};
 

	void OnPickUp();
 
 
};

#endif


