#ifndef _J1ENTITY_LOOT_HEALTH_H__
#define _J1ENTITY_LOOT_HEALTH_H__

#include "j1EntityLoot.h"

class j1EntityLootHealth: public j1EntityLoot
{

public:
	j1EntityLootHealth(float posX, float posY, LOOT_TYPE loot_type, std::string name);
	~j1EntityLootHealth() {};


	void OnPickUp();


};

#endif


