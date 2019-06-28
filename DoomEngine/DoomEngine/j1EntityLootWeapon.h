#ifndef _J1ENTITY_LOOT_H__
#define _J1ENTITY_LOOT_H__

#include "j1EntityLoot.h"
#include <map>

enum class WEAPON_TYPE   
{
	DEFAULT,
	CHAINSAW,
	SHOTGUN,
	NO_WEAPON      // add stuff here 

};

enum class WEAPON_STATE  
{
	AWAIT,
	ACTIVE,
	INACTIVE
};

enum class firingType
{
	AUTO,
	SEMI,  // for the sake of simplicity semi and manual will be the same
	MELEE,
	NO_FIRING_TYPE
};


	const std::map<std::string, WEAPON_TYPE> weaponTypeMap =
	{
		{"default", WEAPON_TYPE::DEFAULT},
		{"chainsaw", WEAPON_TYPE::CHAINSAW},
		{"shotgun", WEAPON_TYPE::SHOTGUN},
		{"", WEAPON_TYPE::NO_WEAPON}
	};
	

	const std::map<std::string, firingType> weaponFiringTypeMap =
	{
		{"auto", firingType::AUTO},
		{"semi", firingType::SEMI},
		{"melee", firingType::MELEE},
		{"", firingType::NO_FIRING_TYPE}
	};

// TODO: audio map according with weapon type ?


struct  weaponInfo
{
	WEAPON_TYPE weaponType; 
	WEAPON_STATE weaponState; 
	firingType FiringType;

int damage = 0;
int cadence = 0;
int maxBullets = 0;

fPoint offsetFromPlayer = fPoint(0.f,0.f); 
float extraOffsetXPlayerIdle = 0.f; 
int xDisplacementWhenRotated = 10; 
};

class j1EntityLoot;
class Collider; 
class j1EntityPlayer; 

class j1EntityLootWeapon : public j1EntityLoot
{

public:
	j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, weaponInfo weaponData);
	j1EntityLootWeapon() {};
	~j1EntityLootWeapon();

	void OnCollision(Collider* c1, Collider* c2) override {};
	void OnCollisionExit(Collider* c1, Collider* c2) override {};

	WEAPON_TYPE GetWeaponType() { return weaponData.weaponType; }; // define this in weapon cpp
	firingType GetFiringType() { return weaponData.FiringType; }; 


	void PlaceMeWithPlayer(); 
	void Shoot(); 
	void ChangeRotation(double angle); 

private:
	weaponInfo weaponData; 

	friend class j1EntityPlayer; 
};

#endif

