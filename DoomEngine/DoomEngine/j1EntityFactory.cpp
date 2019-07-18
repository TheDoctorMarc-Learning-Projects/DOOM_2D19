#include "j1EntityFactory.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Particles.h"
#include "j1PathFinding.h"
#include "j1Input.h"

#include "Brofiler/Brofiler.h"

#include <ctime>
#include <algorithm>

#include "j1EntityPlayer.h"
#include "j1EntityPlatform.h"
#include "j1EnemyIMP.h"
#include "j1EnemyCacodemon.h"
#include "j1EnemyBaronOfHell.h"

#include "LootWeaponMaps.h"

#include "j1BloodManager.h"
#include "j1EntityBloodDrop.h"

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

	entityTextureMap.insert(std::pair("player", App->tex->Load("textures/player/player.png")));
	entityTextureMap.insert(std::pair("EnemyIMP", App->tex->Load("textures/enemies/IMP/IMP.png")));
	entityTextureMap.insert(std::pair("EnemyCacodemon", App->tex->Load("textures/enemies/Cacodemon/Cacodemon.png")));
	entityTextureMap.insert(std::pair("EnemyBaronOfHell", App->tex->Load("textures/enemies/BaronOfHell/BaronOfHell.png")));

	
	// for the moment, create player here 
	player = (j1EntityPlayer*)CreateEntity(PLAYER, 0, 300, "player"); 

	// and test enemies
	//CreateEntity(ENEMY_CACODEMON, 150, 100, "EnemyCacodemon"); 

	//CreateEntity(ENEMY_IMP, 300, 100, "EnemyIMP");

	CreateEntity(ENEMY_BARON_OF_HELL, 300, 100, "EnemyBaronOfHell");


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

	for (auto tex : entityTextureMap)
		App->tex->UnLoad(tex.second); 
	entityTextureMap.clear(); 


	return ret;
}



j1Entity* j1EntityFactory::CreateEntity(ENTITY_TYPE type, int positionX, int positionY, std::string name)
{
	j1Entity* ret = nullptr; 

	switch (type)
	{
	case PLAYER:
		ret = DBG_NEW j1EntityPlayer(positionX, positionY, name); 
		break;
	case ENEMY_IMP:
		ret = DBG_NEW j1EnemyIMP(positionX, positionY, name);
		break;
	case ENEMY_CACODEMON:
		ret = DBG_NEW j1EnemyCacodemon(positionX, positionY, name);
		break;
	case ENEMY_BARON_OF_HELL:
		ret = DBG_NEW j1EnemyBaronOfHell(positionX, positionY, name);
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


void j1EntityFactory::DoDamagetoEntity(j1Entity* ent, uint damage, float cadence, fPoint shotSpeed)
{

	if (ent->to_delete)  // first line prevention 
		return; 

	if (ent->type == PLAYER)   // second line prevention
	{
		if (dynamic_cast<j1EntityPlayer*>(ent)->state.combat == combatState::DYING ||
			dynamic_cast<j1EntityPlayer*>(ent)->state.combat == combatState::DEAD ||  dynamic_cast<j1EntityPlayer*>(ent)->godMode)
			return;
	}
	else
	{

		if (dynamic_cast<j1Enemy*>(ent)->state.combat == eCombatState::DYING || dynamic_cast<j1Enemy*>(ent)->state.combat == eCombatState::DEAD)
		{
			return;
		}
	}
		
	uint previousLife = ent->life; 

	if (ent->type != PLAYER)
	{
		if (player->godMode == true)
			ent->life = 0;
		else
			ent->life -= damage;
	}

	
	if (ent->life <= 0)
	{
		bool brutal = false; 

		if (ent->life <= -EXTRA_DAMAGE_TO_TRIGGER_BRUTAL_DEATH)
			brutal = true; 

		ent->SetDyingState(brutal);
	}
	else
		App->audio->PlayFx(ent->name + "Injured");

	uint bloodDropAmount = App->bloodManager->CalculateNumberOfBloodDropsToBeSpawned(damage, cadence);   // TODO: damage in weapon is affected by scope, so calculate it accordingly

	App->bloodManager->CreateTargetedBloodSteam(ent->collider->rect, 0.5f, bloodDropAmount, shotSpeed);
}

