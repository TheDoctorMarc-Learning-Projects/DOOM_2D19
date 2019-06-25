#ifndef _LOOTENTITY_H__
#define _LOOTENTITY_H__

#include "p2Point.h"
#include "j1Entity.h"



struct SDL_Texture;

enum class LOOT_TYPE
{
	CONSUMABLE,
	WEAPON,
	NO_LOOT
};


enum class WEAPON_TYPE   // TODO:  define this in WEAPON header 
{
    CHAINSAW,
	SHOTGUN,
	NO_WEAPON      // add stuff here 

};

enum class CONSUMABLE_STATE    // TODO:  define this in consumable header 
{
	AWAIT,
	PICKED
};

enum class WEAPON_STATE  // TODO:  define this in WEAPON header 
{
	AWAIT,
	ACTIVE,
	INACTIVE
};

class LootEntity : public j1Entity
{

public:
	LootEntity(LOOT_TYPE type, int posX, int posY);
	LootEntity() {};
	~LootEntity();

	bool CleanUp();

	/*bool Load(pugi::xml_node& node, LootEntity* loot);
	bool Save(pugi::xml_node&) const;*/

	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;

	LOOT_TYPE GetType();
	WEAPON_TYPE GetWeaponType();
	
	fPoint GetPosition();




public:
	
	SDL_Texture* lootTex = nullptr;

private:
	LOOT_TYPE loot_type = LOOT_TYPE::NO_LOOT;




};

#endif
