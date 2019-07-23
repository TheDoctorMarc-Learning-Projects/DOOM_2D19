#ifndef  __J1ENTITY_LOOT_H__
#define __J1ENTITY_LOOT_H__

#include "j1Entity.h"
#include "j1Map.h"

enum class LOOT_TYPE
{
	CONSUMABLE,
	WEAPON,
	NO_LOOT
};

enum class CONSUMABLE_STATE    // TODO:  define this in consumable header 
{
	AWAIT,
	PICKED
};

class j1Entity;

class j1EntityLoot : public j1Entity
{

public:
	j1EntityLoot(float posX, float posY, LOOT_TYPE loot_type, std::string name) :j1Entity(posX, posY) {
		
	
		position = previousPosition = fPoint(posX, posY); 

		type = LOOT;
		this->loot_type = loot_type; 

		entityTex = App->map->entityTextureMap.at("loot"); 
	};

	j1EntityLoot() {};
	~j1EntityLoot() {};

	/*bool Load(pugi::xml_node& node, LootEntity* loot);
	bool Save(pugi::xml_node&) const;*/

	virtual void OnCollision(Collider* c1, Collider* c2) override {};
	virtual void OnCollisionExit(Collider* c1, Collider* c2) override {};

	LOOT_TYPE GetType() { return loot_type; };

private:
	LOOT_TYPE loot_type = LOOT_TYPE::NO_LOOT;

};

#endif // ! __J1ENTITY_LOOT_H__
