#ifndef _J1ENTITY_LOOT_ARMOR_H__
#define _J1ENTITY_LOOT_ARMOR_H__

#include "j1EntityLoot.h"

class j1EntityLootArmor : public j1EntityLoot
{

public:
	j1EntityLootArmor(float posX, float posY, LOOT_TYPE loot_type, std::string name);
	~j1EntityLootArmor() {};


	void OnPickUp();
 

};

#endif
