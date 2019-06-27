#include "j1EntityLootWeapon.h"


j1EntityLootWeapon::j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, weaponInfo weaponData) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->weaponData = weaponData; 

	// capture atlas section according to weapon type, and also the offset from player and collider: 


	switch (weaponData.weaponType)
	{
	case WEAPON_TYPE::CHAINSAW:

		size.create(62, 23); 
		section = {30, 0, size.x, size.y}; 
		
		// TODO: offset form player (when player is holding the weapon)

		break; 
	}

	collider = App->collision->AddCollider(section, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y); 

}

 
j1EntityLootWeapon::~j1EntityLootWeapon()
{

}