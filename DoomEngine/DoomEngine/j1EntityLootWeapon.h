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
class LOOT_TYPE; 

class j1EntityLootWeapon : public j1EntityLoot
{

public:
	j1EntityLootWeapon(int posX, int posY, LOOT_TYPE loot_type) :j1EntityLoot(posX, posY);
	j1EntityLootWeapon() {};
	~j1EntityLootWeapon();

	/*bool CleanUp()
	{
		collider->to_delete = true;
		if (entityTex)
			App->tex->UnLoad(entityTex);
	};*/

	/*bool Load(pugi::xml_node& node, LootEntity* loot);
	bool Save(pugi::xml_node&) const;*/

	void OnCollision(Collider* c1, Collider* c2) override {};
	void OnCollisionExit(Collider* c1, Collider* c2) override {};

	WEAPON_TYPE GetWeaponType(); // define this in weapon cpp


private:
	WEAPON_TYPE weapon_type = WEAPON_TYPE::NO_WEAPON;




};

#endif

