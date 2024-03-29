#ifndef __J1ENTITYFACTORY_H__
#define __J1ENTITYFACTORY_H__

#include "j1Module.h"
 

#include <vector>
#include <list>
#include "Color.h"
#include "j1Map.h"
#include "j1EntityPlayer.h"
#include "j1EntityPlatformDynamic.h"
#include "j1Scene.h"
#include "j1EntityLoot.h"
#include "j1EntityLootWeapon.h"
#include "j1EntityLootCoin.h"
#include "j1EntityLootHealth.h"
#include "j1EntityLootAmmo.h"
#include "j1EntityLootArmor.h"
#include <map>
#include <array>

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

struct DifficultyMultiplier
{
	float enemyDamage = 0.f; 
	float deathTimerUpdate = 0.f;
	float lootAmmout = 0.f; 
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

	void Debug(); 

	void DoDamagetoEntity(j1Entity* ent, float damage, float shotsPerSecond, fPoint shotSpeed = fPoint(0,0));   // to do: capture shot dir and enemy rect for the blood functionality
	void AddLifeToPlayer(float maxLifePercentatge = 0.0f);
	void AddAmmoToPlayer(float maxBulletPercentage = 0.0f); 
	void AddArmorToPlayer(float maxArmorPercentage = 0.0f);

	void SetDifficultyMultiplier(int difficultyLevel);
	void PlayerDeathLogic(); 

	bool Load(pugi::xml_node& node); 
	bool Save(pugi::xml_node& node) const; 


	// entities constructors -------
	 j1Entity* CreateEntity(ENTITY_TYPE type, int positionX, int positionY, std::string name);
	 j1Entity* CreatePlatform(ENTITY_TYPE type, SDL_Rect placing, int heightLevel, std::string name,
		int levelsUp = 0, int LevelsDown = 0, SceneState level = SceneState::LEVEL1, AXIS_Movement movementType = AXIS_Movement::HORIZONTAL);
	 j1Entity* CreateWeapon(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type, weaponInfo weaponData);
	 j1Entity* CreateCoin(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type, bool classic);
	 j1Entity* CreateHealth(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type); 
	 j1Entity* CreateAmmo(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type);
	 j1Entity* CreateArmor(ENTITY_TYPE type, int positionX, int positionY, std::string name, LOOT_TYPE loot_type);

	iPoint GetPlayerPosition() const {
		return iPoint(App->map->WorldToMap((int)player->position.x, (int)player->position.y)) + iPoint(0, 1);
	}

	bool isInDistanceModule(iPoint pos1, iPoint pos2, uint distance)  // tile coords
	{
		uint ret = (uint)(int)(float)abs(hypot(pos1.x - pos2.x, pos1.y - pos2.y));
		return ret <= distance; 
	}

	bool hasIntersectionRectAndLine(const SDL_Rect* rect, std::array<int, 4> line) const  // line is passed like this: {x1, y1, x2, y2}
	{ 
		return SDL_IntersectRectAndLine(rect, &line[0], &line[1], &line[2], &line[3]);
	}

	fPoint GetRectCentralPoint(const SDL_Rect* rect) const {

		return { (float)rect->x + (float)rect->w * 0.5F, (float)rect->y + (float)rect->h * 0.5F };
	}

	bool isPointInsideRect(const iPoint* p, const SDL_Rect* r) const {
		const SDL_Point P = { p->x, p->y }; 
		return SDL_PointInRect(&P, r);
	}

	bool IsPlayerAlive() const { return playerAlive; }; 

	j1Entity* GetEntityFromID(uint ID) const {
		for(const auto& ent : entities)
			if (ent->ID == ID)
			{
				return ent; 
				break; 
			}
		return nullptr; 
	}

private: 

	static bool isBlitOrderHigherThanPreviousEntity(const j1Entity* ent1, const j1Entity* ent2); 

public:

	std::list<j1Entity*>	entities;
	std::map<std::string, SDL_Texture*> entityTextureMap; 
	std::map<std::string, ENTITY_TYPE> enemyTypeMap; 
	std::array<int, 5> platFormLevelHeights; 
	std::array<DifficultyMultiplier, 5> difficultyMultipliers;
	DifficultyMultiplier currentDifficultyMultiplier; 
	j1EntityPlayer* player = nullptr; 
	bool playerAlive = false; 
	uint playerLives = 3; 
	iPoint playerSpawnPos = iPoint(0, 0); 
	uint NumberOfSpawns = 0U; 
    mutable	std::vector<int> saveGameDeletedEntitiesIDs; // quite handy to delete any entity that was destroyed in the previous gameplay
	bool debug = false;


	std::list<encapsulatedEnemyData*> enemiesToBeSpawned; 
	float enemyKillTimeBonusFactor = 2.f;

};




#endif  