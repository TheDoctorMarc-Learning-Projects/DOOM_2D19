#ifndef __J1ENTITYFACTORY_H__
#define __J1ENTITYFACTORY_H__

#include "j1Module.h"
//#include "j1Entity.h"

#include <vector>
#include <list>
#include "Color.h"
#include "j1Map.h"
#include "j1EntityPlayer.h"
#include "j1EntityPlatformDynamic.h"
#include "j1Scene.h"
#include "j1EntityLoot.h"
#include "j1EntityLootWeapon.h"
#include <map>

enum damage_Frame_Type
{
	ONE_SHOT,
	EACH_FRAME
};

struct encapsulatedEnemyData  // needed to load enemies from tiled, because texture map is not available until Start() (cannot be defined in constructor because it needs other modules)
{
	iPoint position;
	std::string name;  // we will comnvert it to a type with the type map easily 
	explicit encapsulatedEnemyData(iPoint pos, std::string name) : position(pos), name(name) {};
};



class j1EntityFactory : public j1Module
{
public:
	j1EntityFactory();
	~j1EntityFactory();

	bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void DoDamagetoEntity(j1Entity* ent, uint damage, float shotsPerSecond, fPoint shotSpeed = fPoint(0,0));   // to do: capture shot dir and enemy rect for the blood functionality

	/*bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;*/  // TODO (xd) 


	// entities constructors -------
	 j1Entity* CreateEntity(ENTITY_TYPE type, int positionX, int positionY, std::string name);
	 j1Entity* CreatePlatform(ENTITY_TYPE type, SDL_Rect placing, int heightLevel, std::string name,
		int levelsUp = 0, int LevelsDown = 0, SceneState level = SceneState::LEVEL1, AXIS_Movement movementType = AXIS_Movement::HORIZONTAL);
	 j1Entity* CreateWeapon(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type, weaponInfo weaponData);

	bool isDistanceToManhattan(iPoint tilePos, iPoint targetTilePos, int distance)
	{
		int distanceManh = tilePos.DistanceManhattan(targetTilePos); 
		return distanceManh <= distance;
	}
	iPoint GetPlayerPosition() const {
		return iPoint(App->map->WorldToMap((int)player->position.x, (int)player->position.y)) + iPoint(0, 1);
	}

	bool isInDistanceModule(iPoint pos1, iPoint pos2, uint distance)
	{
		uint ret = (uint)(int)(float)abs(hypot(pos1.x - pos2.x, pos1.y - pos2.y));
		return ret <= distance; 
	}

private: 

	static bool isBlitOrderHigherThanPreviousEntity(const j1Entity* ent1, const j1Entity* ent2); 

	/*Enemy* CreateEnemy(EnemyType etype, iPoint pos, bool dummy = false);                            // TODO: rework this when enemies available
	std::vector<j1Entity*> CreateEnemiesGroup(std::vector<EnemyType> enemyTypes, SDL_Rect zone, uint minNum, uint maxNum, uint groupLevel = 0);*/
	//void LoadSpawnGroups();

 


public:


	std::list<j1Entity*>	entities;
	std::map<std::string, SDL_Texture*> entityTextureMap; 
	std::map<std::string, ENTITY_TYPE> enemyTypeMap; 
	j1EntityPlayer* player = nullptr; 

	std::list<encapsulatedEnemyData*> enemiesToBeSpawned; 

};




#endif  