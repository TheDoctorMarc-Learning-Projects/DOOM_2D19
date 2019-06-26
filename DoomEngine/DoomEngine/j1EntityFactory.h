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


	/*bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;*/  // TODO (xd) 


	// entities constructors -------
	j1Entity* CreateEntity(ENTITY_TYPE type, int positionX, int positionY, std::string name);
	j1Entity* CreatePlatform(ENTITY_TYPE type, SDL_Rect placing, int heightLevel, std::string name,
		int levelsUp = 0, int LevelsDown = 0, SceneState level = SceneState::LEVEL1, AXIS_Movement movementType = AXIS_Movement::HORIZONTAL);

	/*Enemy* CreateEnemy(EnemyType etype, iPoint pos, bool dummy = false);                            // TODO: rework this when enemies available
	std::vector<j1Entity*> CreateEnemiesGroup(std::vector<EnemyType> enemyTypes, SDL_Rect zone, uint minNum, uint maxNum, uint groupLevel = 0);*/
	//void LoadSpawnGroups();

 


public:
	fPoint					dmg_vec;
	SDL_Texture*			lootTexture = nullptr;
	SDL_Texture*			platfTexture = nullptr;
	SDL_Texture*			playerTexture = nullptr;
	SDL_Texture*			impTexture = nullptr;

	//std::vector<GroupInfo> spawngroups;
	std::list<j1Entity*>	entities;
	j1EntityPlayer* player = nullptr; 

	

};




#endif  