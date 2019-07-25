#ifndef _J1ENTITY_LOOT_AMMO_H__
#define _J1ENTITY_LOOT_AMMO_H__

#include "j1EntityLoot.h"

class j1EntityLootAmmo : public j1EntityLoot
{

public:
	j1EntityLootAmmo(float posX, float posY, LOOT_TYPE loot_type, std::string name);
	~j1EntityLootAmmo() {};


	void OnPickUp();


};

#endif



