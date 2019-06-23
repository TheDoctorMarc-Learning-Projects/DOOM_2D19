#include "j1EntityFactory.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1ParticlesClassic.h"
#include "j1PathFinding.h"

#include "Brofiler/Brofiler.h"

#include <ctime>
#include <algorithm>

#include "j1EntityPlayer.h"
#include "j1EntityPlatform.h"
#include "j1EnemyIMP.h"

#include <assert.h>

j1EntityFactory::j1EntityFactory()
{
	name.assign("entities");

}

j1EntityFactory::~j1EntityFactory()
{
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

	// TODO: Load Atlas 

	
	
	// for the moment, create player here 
	player = (j1EntityPlayer*)CreateEntity(PLAYER, 0, 0, "player"); 

	// and test enemies
	CreateEntity(ENEMY_IMP, 100, 100, "imp"); 



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
				ret = ((*item)->Move(dt));

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

	// TODO: draw all in-camera entities: call each entity's Draw() 


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


/*Enemy * j1EntityFactory::CreateEnemy(EnemyType etype,iPoint pos, bool dummy)
{
	Enemy* ret = nullptr; 

	switch (etype)
	{
	case EnemyType::BOMB:
		ret = DBG_NEW EnemyBomb(pos, dummy);
		entities.push_back(ret);
		break;
	case EnemyType::TEST:
		ret = DBG_NEW EnemyTest(pos, dummy);
		entities.push_back(ret);
		break; 

	case EnemyType::MELEE:
		break; 

	case EnemyType::ARCHER:
		ret = DBG_NEW EnemyArcher(pos, dummy); 
		entities.push_back(ret);
		break; 

	case EnemyType::DUMMY:
		ret = DBG_NEW EnemyDummy(pos); 
		entities.push_back(ret); 
		break;

	default:
		break;
	}
	

	return ret;
}*/

/*std::vector<j1Entity*> j1EntityFactory::CreateEnemiesGroup(std::vector<EnemyType> enemyTypes, SDL_Rect zone, uint minNum, uint maxNum, uint groupLevel)
{

	std::vector<j1Entity*> return_vector;

	uint numEnemies = CreateRandomBetween(minNum, maxNum);

	uint numBombs = 0;
	uint numTests = 0;
	uint numArchers = 0; 
	uint cont = 0;

	uint bombProbs = 3;
	uint testProbs = 8;
	uint archerProbs = 3;

	if (enemyTypes.empty())
		return return_vector;

	while (cont < numEnemies)
	{
		for (std::vector<EnemyType>::iterator typeIter = enemyTypes.begin(); typeIter != enemyTypes.end(); typeIter++)
		{
			Enemy* ret = nullptr;

			// If the level is passed as paramater (read through tiled), add this level, if not, create a random 2 above the player
			int enemyLevel = 0;
			groupLevel != 0 ? enemyLevel = groupLevel : enemyLevel = CreateRandomBetween(0, 2); 

			iPoint spawnPos = { zone.x + (int)CreateRandomBetween(0, zone.w), zone.y + (int)CreateRandomBetween(0,zone.h) };
			spawnPos = App->map->IsoToWorld(spawnPos.x, spawnPos.y);
			spawnPos.x = spawnPos.x * 2;
			if (!App->pathfinding->IsWalkable(App->map->WorldToMap(spawnPos.x, spawnPos.y)))
				continue; 

			LOG("Spawn Position: %i, %i", spawnPos.x, spawnPos.y);

			switch (*typeIter)
			{
			case EnemyType::BOMB:
				if (CreateRandomBetween(1, 10) <= bombProbs && cont < numEnemies)
				{
					// Last paramater is dummy
					ret = CreateEnemy(EnemyType::BOMB, spawnPos, false);

					if (App->entityFactory->player != nullptr)
					{
						ret->level = App->entityFactory->player->level + enemyLevel;
					}
					if (ret != nullptr)
					{
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::ATTACK_ROL, ret, "\0", CreateRandomBetween(3, 5) + 5 * ret->level);
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::DEFENCE_ROL, ret, "\0", CreateRandomBetween(4, 8) + 5 * ret->level);
						numBombs++;
						cont++;
					}
				}

				break;

			case EnemyType::TEST:
				if (CreateRandomBetween(1, 10) <= testProbs && cont < numEnemies)
				{
					// Last paramater is dummy
					ret = CreateEnemy(EnemyType::TEST, spawnPos, false);

					if (App->entityFactory->player != nullptr)
					{
						ret->level = App->entityFactory->player->level + enemyLevel;
					}
					if (ret != nullptr)
					{
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::ATTACK_ROL, ret, "\0", CreateRandomBetween(2, 3) + 5 * ret->level);
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::DEFENCE_ROL, ret, "\0", CreateRandomBetween(4, 8) + 5 * ret->level);
						numTests++;
						cont++;
					}
				}
				break;

			case EnemyType::ARCHER:
				if (CreateRandomBetween(1, 10) <= archerProbs && cont < numEnemies)
				{
					// Last paramater is dummy
					ret = CreateEnemy(EnemyType::ARCHER, spawnPos, false);

					if (App->entityFactory->player != nullptr)
					{
						ret->level = App->entityFactory->player->level + enemyLevel;
					}
					if (ret != nullptr)
					{
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::ATTACK_ROL, ret, "\0", CreateRandomBetween(4, 6) + 6 * ret->level);
						App->buff->CreateBuff(BUFF_TYPE::ADDITIVE, ELEMENTAL_TYPE::ALL_ELEMENTS, ROL::DEFENCE_ROL, ret, "\0", CreateRandomBetween(20, 25) + 6 * ret->level);
						numArchers++;
						cont++;
					}
				}
				break;

			default:
				break;
			}

			// push to vector
			if (ret != nullptr)
				return_vector.push_back(ret);

		}
	}
	LOG("Created %i Enemies", numEnemies);
	LOG("Ceated %i TESTS", numTests);
	LOG("Ceated %i BOMBS", numBombs);

	return return_vector;
}*/

/*void j1EntityFactory::LoadSpawnGroups()
{
	if (!App->scene->ComeToPortal) {
		for (std::vector<GroupInfo>::iterator iter = spawngroups.begin(); iter != spawngroups.end(); iter++) {
			CreateEnemiesGroup((*iter).types, (*iter).zone, (*iter).minEnemies, (*iter).maxEnemies, (*iter).groupLevel);
		}
	}
	spawngroups.clear();
}*/

