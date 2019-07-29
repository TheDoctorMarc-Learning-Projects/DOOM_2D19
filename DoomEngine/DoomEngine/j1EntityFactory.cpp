#include "j1EntityFactory.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Particles.h"
#include "j1PathFinding.h"
#include "j1Input.h"

#include "Brofiler/Brofiler.h"

#include <algorithm>

#include "j1EntityPlayer.h"
#include "j1EntityPlatform.h"
#include "j1EnemyIMP.h"
#include "j1EnemyCacodemon.h"
#include "j1EnemyBaronOfHell.h"
#include "j1EnemyHellKnight.h"

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

	// TODO: textures

	entityTextureMap.insert(std::make_pair("player", App->tex->Load("textures/player/player.png")));
	entityTextureMap.insert(std::make_pair("EnemyIMP", App->tex->Load("textures/enemies/IMP/IMP.png")));
	entityTextureMap.insert(std::make_pair("EnemyCacodemon", App->tex->Load("textures/enemies/Cacodemon/Cacodemon.png")));
	entityTextureMap.insert(std::make_pair("EnemyBaronOfHell", App->tex->Load("textures/enemies/BaronOfHell/BaronOfHell.png")));
	entityTextureMap.insert(std::make_pair("EnemyHellKnight", App->tex->Load("textures/enemies/HellKnight/HellKnight.png")));

 
	enemyTypeMap.insert(std::make_pair("EnemyIMP", ENEMY_IMP)); 
	enemyTypeMap.insert(std::make_pair("EnemyCacodemon", ENEMY_CACODEMON));
	enemyTypeMap.insert(std::make_pair("EnemyBaronOfHell", ENEMY_BARON_OF_HELL));
	enemyTypeMap.insert(std::make_pair("EnemyHellKnight", ENEMY_HELL_KNIGHT));

	// TODO: KEEP UPDATING THIS with new types of enemies... 

	// ENEMIES (tiled): 

	for (auto enemyData : enemiesToBeSpawned)
		CreateEntity(enemyTypeMap.at(enemyData->name), enemyData->position.x, enemyData->position.y, enemyData->name); 

	for (auto enemyData = enemiesToBeSpawned.begin(); enemyData != enemiesToBeSpawned.end(); ++enemyData)
		RELEASE(*enemyData); 
	enemiesToBeSpawned.clear(); 
	
	// for the moment, create player here 
	player = (j1EntityPlayer*)CreateEntity(PLAYER, 0, 300, "player"); 


	std::list<j1Entity*>::iterator item = entities.begin();
	for (; item != entities.end(); ++item)
	{
		if ((*item) != nullptr)
			(*item)->Start();
	}

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

	std::vector<j1Entity*> blitOrderEntities{ std::begin(entities), std::end(entities) };

	std::sort(blitOrderEntities.begin(), blitOrderEntities.end(), j1EntityFactory::isBlitOrderHigherThanPreviousEntity);

	for (auto entity : blitOrderEntities)
		if (!entity->to_delete)
			if (App->render->IsOnCamera(entity->position.x, entity->position.y, entity->size.x, entity->size.y))
				entity->Draw();
		

	blitOrderEntities.clear(); 

	 
	Debug(); 
				 
				
	return true;
}

bool j1EntityFactory::isBlitOrderHigherThanPreviousEntity(const j1Entity* ent1, const j1Entity* ent2)
{
	return ent2->blitOrder > ent1->blitOrder;
}


void j1EntityFactory::Debug()
{

	// - - - - - - - - - - - - - - - - debug


	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		debug = !debug; 

	if (debug)
	{
		for(const auto& entity : entities)
			if(entity->isEnemy)
				if(dynamic_cast<j1Enemy*>(entity)->state.combat != eCombatState::DYING && dynamic_cast<j1Enemy*>(entity)->state.combat != eCombatState::DEAD)
					App->render->DrawLine(dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.lastRaycast[0],
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.lastRaycast[1],
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.lastRaycast[2],
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.lastRaycast[3],
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.Color.r,
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.Color.g,
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.Color.b,
						dynamic_cast<j1Enemy*>(entity)->lastRaycastInfo.Color.a,
						true);
				
	}
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
	{
		App->tex->UnLoad(tex.second);
		tex.second = nullptr; 
	}
	
	entityTextureMap.clear();
	enemyTypeMap.clear(); 

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
	case ENEMY_HELL_KNIGHT:
		ret = DBG_NEW j1EnemyHellKnight(positionX, positionY, name);
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

	ret = DBG_NEW j1EntityLootWeapon(positionX, positionY, LOOT_TYPE::WEAPON, name, weaponData);

	if (ret)
		entities.push_back(ret);

	return ret;
}


j1Entity* j1EntityFactory::CreateCoin(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE lootType, bool classic)
{
	j1Entity* ret = nullptr;

	ret = DBG_NEW j1EntityLootCoin(positionX, positionY, LOOT_TYPE::COIN, name, classic);

	if (ret)
		entities.push_back(ret);

	return ret;
}


j1Entity* j1EntityFactory::CreateHealth(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE lootType)
{
	j1Entity* ret = nullptr;

	ret = DBG_NEW j1EntityLootHealth(positionX, positionY, LOOT_TYPE::HEALTH, name);

	if (ret)
		entities.push_back(ret);

	return ret;
}



j1Entity* j1EntityFactory::CreateAmmo(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE lootType)
{
	j1Entity* ret = nullptr;

	ret = DBG_NEW j1EntityLootAmmo(positionX, positionY, LOOT_TYPE::AMMO, name);

	if (ret)
		entities.push_back(ret);

	return ret;
}



j1Entity* j1EntityFactory::CreateArmor(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE lootType)
{
	j1Entity* ret = nullptr;

	ret = DBG_NEW j1EntityLootArmor(positionX, positionY, LOOT_TYPE::ARMOR, name);

	if (ret)
		entities.push_back(ret);

	return ret;
}


void j1EntityFactory::DoDamagetoEntity(j1Entity* ent, float damage, float cadence, fPoint shotSpeed)
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
		
	float previousLife = ent->life;

	if (ent->type != PLAYER)
	{
		if (player->godMode == true)  // if godmode is active, just kill the enemy 
			ent->life = 0.f;
		else
			ent->life -= damage;
	}
	else
	{
		
		if (player->armor > 0.0f)
		{
			float captureArmor = player->armor;     // apply damage to the armor first 
			float extraDamage = 0.0f;

			captureArmor -= damage;

			if (captureArmor >= 0.0f)
				player->armor = captureArmor; 
			else
			{
				player->armor = 0.0f; 
				extraDamage = abs(captureArmor);
				player->life -= extraDamage; 
			}
				
		}
		else
			player->life -= damage;     

		
			
	}
		

	
	if (ent->life <= 0.f)
	{
		bool brutal = false; 

		if (ent->life <= -EXTRA_DAMAGE_TO_TRIGGER_BRUTAL_DEATH)  // if life exceeds a negative value, it is considered as a brutal death
			brutal = true; 

		ent->SetDyingState(brutal);
	}
	else
		App->audio->PlayFx(ent->name + "Injured");

	uint bloodDropAmount = App->bloodManager->CalculateNumberOfBloodDropsToBeSpawned(damage, cadence); 

	App->bloodManager->CreateTargetedBloodSteam(ent->collider->rect, 0.5f, bloodDropAmount, shotSpeed);
}



void j1EntityFactory::AddLifeToEntity(j1Entity* ent, float maxLifePercentatge)
{
	float captureLife = ent->life;

	captureLife += (maxLifePercentatge * ent->maxLife);

	if (captureLife > ent->maxLife)
		captureLife -= (captureLife - ent->maxLife);

	ent->life = captureLife;
}


void j1EntityFactory::AddAmmoToPlayer(float maxBulletPercentage)
{
	if(player->myWeapons.empty() == false)
		for (auto& weapon : player->myWeapons)
		{
			if (weapon->maxBullets == 0.0f)
				return; 

			int bullets = weapon->currentBullets;

			bullets += (int)(maxBulletPercentage * (float)weapon->maxBullets);

			if (bullets > weapon->maxBullets)
				bullets -= (bullets - weapon->maxBullets);

			weapon->currentBullets = bullets;
		}
	

}

void j1EntityFactory::AddArmorToPlayer(float maxArmorPercentatge)
{
	float captureArmor = player->armor;

	captureArmor += (maxArmorPercentatge * player->maxArmor);

	if (captureArmor > player->maxArmor)
		captureArmor -= (captureArmor - player->maxArmor);

	player->armor = captureArmor;
}