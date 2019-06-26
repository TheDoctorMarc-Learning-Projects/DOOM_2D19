#include "j1EntityLootWeapon.h"


j1EntityLootWeapon::j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, WEAPON_TYPE weapon_type) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->weapon_type = weapon_type; 
}

 
j1EntityLootWeapon::~j1EntityLootWeapon()
{

}