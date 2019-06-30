#include <map>
#include "j1EntityLootWeapon.h"

const std::map<std::string, WEAPON_TYPE> weaponTypeMap =
{
	{"default", WEAPON_TYPE::DEFAULT},
	{"chainsaw", WEAPON_TYPE::CHAINSAW},
	{"shotgun", WEAPON_TYPE::SHOTGUN},
	{"machineGun", WEAPON_TYPE::MACHINE_GUN},
	{"", WEAPON_TYPE::NO_WEAPON}
};


const std::map<std::string, firingType> weaponFiringTypeMap =
{
	{"auto", firingType::AUTO},
	{"semi", firingType::SEMI},
	{"melee", firingType::MELEE},
	{"", firingType::NO_FIRING_TYPE}
};


// TODO: audio and particle map according with weapon type ?

static void CleanLootMaps() 
{
	const_cast<std::map<std::string, WEAPON_TYPE>&>(weaponTypeMap).clear();
	const_cast<std::map<std::string, firingType>&>(weaponFiringTypeMap).clear();
}
