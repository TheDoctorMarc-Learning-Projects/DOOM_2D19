#ifndef _J1ENTITY_LOOT_H__
#define _J1ENTITY_LOOT_H__

#include "j1EntityLoot.h"

enum class WEAPON_TYPE   // TODO:  define this in WEAPON header 
{
	DEFAULT,
	CHAINSAW,
	SHOTGUN,
	NO_WEAPON      // add stuff here 

};

enum class WEAPON_STATE  // TODO:  define this in WEAPON header 
{
	AWAIT,
	ACTIVE,
	INACTIVE
};

class j1EntityLoot;
class Collider; 


class j1EntityLootWeapon : public j1EntityLoot
{

public:
	j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, WEAPON_TYPE weapon_type);
	j1EntityLootWeapon() {};
	~j1EntityLootWeapon();

	void OnCollision(Collider* c1, Collider* c2) override {};
	void OnCollisionExit(Collider* c1, Collider* c2) override {};

	WEAPON_TYPE GetWeaponType() { return weapon_type; }; // define this in weapon cpp


private:
	WEAPON_TYPE weapon_type = WEAPON_TYPE::NO_WEAPON;




};

#endif

