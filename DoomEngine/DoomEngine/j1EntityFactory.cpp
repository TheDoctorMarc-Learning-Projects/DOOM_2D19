#include "j1EntityFactory.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1ParticlesClassic.h"
#include "j1PathFinding.h"
#include "j1Input.h"

#include "Brofiler/Brofiler.h"

#include <ctime>
#include <algorithm>

#include "j1EntityPlayer.h"
#include "j1EntityPlatform.h"
#include "j1EnemyIMP.h"

#include "LootWeaponMaps.h"

#include <assert.h>

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");

}

j1EntityFactory::~j1EntityFactory()
{
	CleanLootMaps();    // they are defined it their header, but are useful until app closes
}

bool j1EntityFactory::Awake(pugi::xml_node & node)
{
bool ret = true;

return ret;
}

bool j1EntityFactory::Start()
{


	std::list<j1Entity*>::iterator item = entities.begin();
	for (; item != entities.end(); ++item)
	{
		if ((*item) != nullptr)
			(*item)->Start();
	}

	// TODO: textures



	playerTexture = App->tex->Load("textures/player/player.png");
	impTexture = App->tex->Load("textures/enemies/IMP/IMP.png");
	
	// for the moment, create player here 
	player = (j1EntityPlayer*)CreateEntity(PLAYER, 0, 0, "player"); 

	// and test enemies
	CreateEntity(ENEMY_IMP, 150, 100, "imp"); 



	return true;
}

bool j1EntityFactory::PreUpdate()
{
	BROFILER_CATEGORY("Entities PreUpdate", Profiler::Color::LavenderBlush);

	bool ret = true;

	// logic / collisions
	std::list<j1Entity*>::iterator item = entities.begin();
	for (; item != entities.end(); ++item)
	{
		if ((*item) != nullptr)
		{
			ret = ((*item)->PreUpdate());
		}
	}

	return true;
}

bool j1EntityFactory::Update(float dt)
{
	bool ret = true;
	BROFILER_CATEGORY("Entities Update", Profiler::Color::Fuchsia);


	/*// testinpurposes 
	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN)
	{
		int x, y;
		x = y = 0;
		App->input->GetMousePosition(x, y);
		CreateEntity(ENEMY_IMP, x, y, "imp");
	}*/




	std::list<j1Entity*>::iterator item = entities.begin();
	for (; item != entities.end();)
	{
		if ((*item) != nullptr)
		{
			if (!(*item)->to_delete)
			{
				
					ret = (*item)->Update(dt);
					ret = (*item)->Move(dt);

					++item;
			
			}
			else
			{
				(*item)->CleanUp();
				delete(*item);
				(*item) = nullptr;
				item = entities.erase(item);
			}

		}
		else
		{
			item = entities.erase(item);
		}

	}


	return ret;
}

bool j1EntityFactory::PostUpdate()
{
	BROFILER_CATEGORY("Entities PostUpdate", Profiler::Color::BurlyWood);

	for (auto entity : entities)
		if (!entity->to_delete)
			if (App->render->IsOnCamera(entity->position.x, entity->position.y, entity->size.x, entity->size.y))
				entity->Draw(); 
				
	return true;
}

bool j1EntityFactory::CleanUp()
{
	bool ret = true;

	std::list<j1Entity*>::reverse_iterator entitiesItem = entities.rbegin();

	while (entitiesItem != entities.rend())
	{
		if ((*entitiesItem) != nullptr)
		{
			(*entitiesItem)->CleanUp();
			RELEASE(*entitiesItem);
			*entitiesItem = nullptr;
		}
		++entitiesItem;

	}
	entities.clear();

	// TODO: unload Atlas texture

	App->tex->UnLoad(playerTexture);
	App->tex->UnLoad(impTexture);

	return ret;
}



j1Entity* j1EntityFactory::CreateEntity(ENTITY_TYPE type, int positionX, int positionY, std::string name)
{
	j1Entity* ret = nullptr; 

	switch (type)
	{
	case PLAYER:
		ret = DBG_NEW j1EntityPlayer(positionX, positionY); 
		break;
	case ENEMY_IMP:
		ret = DBG_NEW j1EnemyIMP(positionX, positionY);
		break;

	default:
		break;
	}

	if (ret)
		entities.push_back(ret); 

	return ret;
}

j1Entity* j1EntityFactory::CreatePlatform(ENTITY_TYPE type, SDL_Rect placing, int heightLevel, std::string name, int levelsUp, int LevelsDown, SceneState level, AXIS_Movement movementType)
{

	j1Entity* ret = nullptr;

	switch (type)
	{

	case ENTITY_STATIC:
		ret = DBG_NEW j1EntityPlatform(placing, heightLevel);
		break;
	case ENTITY_DYNAMIC:
		ret = DBG_NEW j1EntityPlatformDynamic(placing, heightLevel, levelsUp, LevelsDown, level, movementType);
		break;


	default:
		break;
	}

	if (ret)
		entities.push_back(ret);

	return ret;
}

j1Entity* j1EntityFactory::CreateWeapon(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type, weaponInfo weaponData)
{
	j1Entity* ret = nullptr;

	ret = DBG_NEW j1EntityLootWeapon(positionX, positionY, loot_type, name, weaponData);

	if (ret)
		entities.push_back(ret);

	return ret;
}

