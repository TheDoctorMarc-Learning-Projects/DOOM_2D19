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
#include "j1Gui.h"

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

    difficultyMultipliers.at(0).enemyDamage = 0.45f; 
	difficultyMultipliers.at(0).deathTimerUpdate = 3.f;
	difficultyMultipliers.at(0).lootAmmout = 2.f;

	difficultyMultipliers.at(1).enemyDamage = 0.85f;
	difficultyMultipliers.at(1).deathTimerUpdate = 1.5f;
	difficultyMultipliers.at(1).lootAmmout = 1.5f;

	difficultyMultipliers.at(2).enemyDamage = 1.f;                   // normal difficulty 
	difficultyMultipliers.at(2).deathTimerUpdate = 1.f;
	difficultyMultipliers.at(2).lootAmmout = 1.f;

	difficultyMultipliers.at(3).enemyDamage = 1.5f;
	difficultyMultipliers.at(3).deathTimerUpdate = 1.f;
	difficultyMultipliers.at(3).lootAmmout = 0.5f;

	difficultyMultipliers.at(4).enemyDamage = 2.5f;
	difficultyMultipliers.at(4).deathTimerUpdate = 0.8f;
	difficultyMultipliers.at(4).lootAmmout = 0.2f;

	currentDifficultyMultiplier = difficultyMultipliers.at(2); 

return ret;
}

bool j1EntityFactory::Start()
{

	entityTextureMap.insert(std::make_pair("player", App->tex->Load("textures/player/player.png")));
	entityTextureMap.insert(std::make_pair("EnemyIMP", App->tex->Load("textures/enemies/IMP/IMP.png")));
	entityTextureMap.insert(std::make_pair("EnemyCacodemon", App->tex->Load("textures/enemies/Cacodemon/Cacodemon.png")));
	entityTextureMap.insert(std::make_pair("EnemyBaronOfHell", App->tex->Load("textures/enemies/BaronOfHell/BaronOfHell.png")));
	entityTextureMap.insert(std::make_pair("EnemyHellKnight", App->tex->Load("textures/enemies/HellKnight/HellKnight.png")));


	enemyTypeMap.insert(std::make_pair("EnemyIMP", ENEMY_IMP));
	enemyTypeMap.insert(std::make_pair("EnemyCacodemon", ENEMY_CACODEMON));
	enemyTypeMap.insert(std::make_pair("EnemyBaronOfHell", ENEMY_BARON_OF_HELL));
	enemyTypeMap.insert(std::make_pair("EnemyHellKnight", ENEMY_HELL_KNIGHT));


	// ENEMIES (tiled): 

	for (auto enemyData : enemiesToBeSpawned)
		CreateEntity(enemyTypeMap.at(enemyData->name), enemyData->position.x, enemyData->position.y, enemyData->name); 

	for (auto enemyData = enemiesToBeSpawned.begin(); enemyData != enemiesToBeSpawned.end(); ++enemyData)
		RELEASE(*enemyData); 
	enemiesToBeSpawned.clear(); 
	
	// for the moment, create player here 
	player = (j1EntityPlayer*)CreateEntity(PLAYER, playerSpawnPos.x, playerSpawnPos.y, "player");

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

	if (App->pause == true)
		return true;

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
	BROFILER_CATEGORY("Entities Update", Profiler::Color::Fuchsia);

	if (App->pause == true)
		return true; 

	std::list<j1Entity*>::iterator item = entities.begin();
	for (; item != entities.end();)
	{
		if ((*item) != nullptr)
		{
			if (!(*item)->to_delete)
			{
				
					(*item)->Update(dt);
					(*item)->Move(dt);

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


	return true;
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


	if (App->pause == true)
		return true;

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

	if (debug == true)
	{
		for(const auto& entity : entities)
			if(entity->isEnemy)
				if(App->render->IsOnCamera(entity->position.x, entity->position.y, entity->collider->rect.w, entity->collider->rect.h))
					if (dynamic_cast<j1Enemy*>(entity)->state.combat != eCombatState::DYING && dynamic_cast<j1Enemy*>(entity)->state.combat != eCombatState::DEAD)
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

	// - - - - - - - - - - - - - - - - - - - - - - - - - 1) Preventions

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

	// - - - - - - - - - - - - - - - - - - - - - - - - - 2) Do damage

	if (ent->type != PLAYER)
	{
		if (player->godMode == true)  // if godmode is active, just kill the enemy 
			ent->life = 0.f;
		else
			ent->life -= damage;

	}
	else
	{
		// take into account the difficulty
		damage *= currentDifficultyMultiplier.enemyDamage; 

		float previousArmor = player->armor;
		float previousLife = player->life;
		
		if (player->armor > 0.0f)
		{
			   // apply damage to the armor first 
			float captureArmor = player->armor;
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

		// warn the GUI

		if(previousArmor != player->armor)
			App->gui->UpDateInGameUISlot("armorLabel", player->armor);
		if (previousLife != player->life)
			App->gui->UpDateInGameUISlot("healthLabel", player->life);

		// warn the GUI
		dynamic_cast<UiItem_Face*>(App->gui->GetItemByName("face"))->SetCurrentAnim("damaged");
			
	}
		
	// - - - - - - - - - - - - - - - - - - - - - - - - - 3) Set the dying state 
	
	if (ent->life <= 0.f)
	{
		bool brutal = false; 

		if (ent->life <= -EXTRA_DAMAGE_TO_TRIGGER_BRUTAL_DEATH)  // if life exceeds a negative value, it is considered as a brutal death
			brutal = true; 

		ent->SetDyingState(brutal);


		if (ent != player)  
		{
			// add time to the death countdown
			App->gui->UpdateDeathTimer((int)((float)(int)dynamic_cast<j1Enemy*>(ent)->powerLevel * enemyKillTimeBonusFactor * currentDifficultyMultiplier.deathTimerUpdate)); 

			// warn the GUI
			dynamic_cast<UiItem_Face*>(App->gui->GetItemByName("face"))->SetCurrentAnim("kill");
		}
		else
		{

			playerAlive = false;
			playerLives--;

			// warn the GUI
			dynamic_cast<UiItem_Face*>(App->gui->GetItemByName("face"))->SetCurrentAnim("death");

			// finally check the remaining lives and switch scene according
			if (playerLives > 0)
			{
				//App->scene->LoadScene(App->scene->GetCurrentSceneState(), true);
				App->scene->SetTargetScene(App->scene->GetCurrentSceneState()); 
				return; 
			}
				
			else
			{
				playerLives = 3; 
			//	App->scene->LoadScene(SceneState::MAINMENU, true);
				App->scene->SetTargetScene(SceneState::MAINMENU);
				return; 
			}
		
		}
			
			
	}
	else
		App->audio->PlayFx(ent->name + "Injured");

	// - - - - - - - - - - - - - - - - - - - - - - - - - 4) spawn the blood 

	uint bloodDropAmount = App->bloodManager->CalculateNumberOfBloodDropsToBeSpawned(damage, cadence); 
	App->bloodManager->CreateTargetedBloodSteam(ent->collider->rect, 0.5f, bloodDropAmount, shotSpeed);
}



void j1EntityFactory::AddLifeToPlayer(float maxLifePercentatge)
{
	float captureLife = player->life;

	captureLife += (maxLifePercentatge * player->maxLife);

	if (captureLife > player->maxLife)
		captureLife -= (captureLife - player->maxLife);

	player->life = captureLife;

	App->gui->UpDateInGameUISlot("healthLabel", player->life);

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
	

	App->gui->UpDateInGameUISlot("ammoLabel", player->currentWeapon->currentBullets);
}

void j1EntityFactory::AddArmorToPlayer(float maxArmorPercentatge)
{
	float captureArmor = player->armor;

	captureArmor += (maxArmorPercentatge * player->maxArmor);

	if (captureArmor > player->maxArmor)
		captureArmor -= (captureArmor - player->maxArmor);

	player->armor = captureArmor;


	App->gui->UpDateInGameUISlot("armorLabel", player->armor);
}


void j1EntityFactory::SetDifficultyMultiplier(int difficultyLevel)
{
	currentDifficultyMultiplier = difficultyMultipliers.at(difficultyLevel - 1); 
}