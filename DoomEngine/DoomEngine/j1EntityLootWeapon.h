#ifndef _J1ENTITY_LOOT_H__
#define _J1ENTITY_LOOT_H__

#include "j1EntityLoot.h"
#include "j1ParticlesClassic.h"
#include <map>

enum class WEAPON_TYPE   
{
	DEFAULT,
	CHAINSAW,
	SHOTGUN,
	MACHINE_GUN,
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



struct  weaponInfo   
{
// - - - - - - - - - - - - global 
	WEAPON_TYPE weaponType; 
	WEAPON_STATE weaponState; 
	firingType FiringType;

// - - - - - - - - - - - - stats 
   int damage = 0;
   int cadence = 0;      // this is captured in shots per minute and then translated 
   int maxBullets = 0;
   float scopeFactor = 0.f;  // the further away, the less damage

// - - - - - - - - - - - - placing 
   fPoint offsetFromPlayer = fPoint(0.f,0.f); 
   float extraOffsetXPlayerIdle = 0.f; 
   int tipPosDisplacement = 0.f; 
   float xDisplacementWhenRotated = 10.f; 

// - - - - - - - - - - - - particle data
   bool launchesProjectile = false; 

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

