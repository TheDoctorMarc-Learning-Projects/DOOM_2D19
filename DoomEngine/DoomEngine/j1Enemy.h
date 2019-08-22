#ifndef _J1ENEMY_H__
#define _J1ENEMY_H__

#include "j1Entity.h"
#include "j1EntityPlatform.h"
#include <vector> 

enum class enemyPathType
{
	ALL_ROUND,
	A_TO_B,
	FLYING
};

enum class eCombatState
{
	IDLE,
	SHOOT,
	STUN,
	DYING,
	DEAD
};

enum class ePathState
{
	FOLLOW_PLAYER,
	TEMPORAL_DEVIATION,
	AWAIT,
};

enum class eMovementState
{
	IDLE,
	RUN,
	INPUT_RIGHT,
	INPUT_LEFT,
	JUMP,
	FALL,
	NOT_ACTIVE
};

struct myEnemyState
{
	eCombatState combat;
	std::array<eMovementState, 2> movement; // 0 for idle, run, 1 for jump or fall
	ePathState path; 

};

enum ATTACK_TYPE
{
	MELEE,
	LONG_RANGE,
	NO_ATTACK_TYPE
};

struct TargetPos
{
	enum targetPosType
	{
		X,
		Y,
		XY,
	};

	targetPosType type = targetPosType::XY;
    fPoint value = fPoint(0, 0); 
};

struct ejumpData
{
	float jumpPower = 38.5f;
	float currenJumpPower = jumpPower;
	float jumpIncrementFactor = .9f;
	float speedXIncrementJump = 2.5f;
	float speedXIncrementFall = 1.2f;
	float verticalIncrementFactor = 1.02f;     // jumpincrement reduces jump power in time, but vertical increment counteracts it a bit 
};


struct baseCadenceValues
{
	uint melee = 0U; 
	uint longRange = 0U; 
};

struct baseDamageValues
{
	uint melee = 0U; 
	uint longRange = 0U; 
};

struct fxAndAnimData
{
	bool hasSecondDeathAnim = false; 
	bool hasSecondDeathFx = false; 
	bool hasSecondAttackFx = false; 
	bool firstAttackFxIsMelee = true; 
};

struct longShotData
{
	uint msWaitFromAnimStartToShot = 0U; 
	iPoint relativeOffsetPos = iPoint(0,0); 
	uint shotSpeed = 0U; 
};

struct AttackData
{
	uint lastTimeMeleeAttack = 0U; 
	uint lastTimeLongRangeAttack = 0U;
	bool lastShooted = false; 
};


struct lastRaycastData
{
	std::array<int, 4> lastRaycast = { 0, 0, 0, 0 } ;
	SDL_Color Color = { 0, 0, 0, 0 }; 
};

class j1Entity;

class j1Enemy : public j1Entity
{
public:
	j1Enemy(int posX, int posY);
	~j1Enemy();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	virtual bool DoDie() {        // not a cleanup, because texture is still there, its just to delete anything extra

		if (onDynamicplatform == false)  // we need to keep the colldier if the enemy dies in the dynamic platform
		{
			if (deathPosGround.IsZero() == false)  // only when arriving to the ground the colldier must be deleted
			{
				if (colliderActive == true)
				{
					colliderActive = false;
					collider->to_delete = true;
				}
			}
			
		}
	
		
		return true;
	};

	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;

private: 
	void VerticalMovement(float dt); 
	bool FollowPath(float dt); 
	bool CheckPathState(iPoint pos, iPoint& target, bool& success); 
	void CallPathCreation(iPoint pos, iPoint target, bool& success); 
	void SolveMove(fPoint direction, float dt); 
	virtual void ResolvePathDeviation() {};
	
	void SetPath(float dt, bool& moveSuccess); 
	bool HasArrivedToTarget(iPoint tilePos, iPoint targetTilePos); 
	void SetPreviousFrameData();
	void SetCollider();
	void DieLogic(float dt); 

public: 

	virtual bool DoMeleeAttack();
	virtual bool DoLongRangeAttack();
	

	bool Go_A_to_B(); 

	virtual POINTING_DIR GetDirection() override; 

	void SetDyingState(bool brutal = false); 

	virtual void CheckDeathFinished() override
	{

		if (currentAnimation->Finished() == true)
		{
			state.combat = eCombatState::DEAD;
			
			//to_delete = true;   // create the corpse here 
			DoDie(); 
		}
	}

	bool isPlayerOnMeleeRange() const; 
	bool isPlayerOnMyZone() const; 
	bool isPlayerOnSamePlatform() const; 

	bool isAiming() const
	{
		if (state.combat == eCombatState::SHOOT)
		{
			if (currentAttackType == ATTACK_TYPE::LONG_RANGE)
			{
				// take into account a possible delay 
				if (SDL_GetTicks() < currentAttackData.lastTimeLongRangeAttack + longRangeShootData.msWaitFromAnimStartToShot && currentAttackData.lastShooted == false)
				{
					return true;
				}

			}
		}

		return false;
	}

	void SpawnShotParticle(); 
	void PutCombatStateToIdle()
	{
		
			currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE;
			state.combat = eCombatState::IDLE;

			if (lastSpeed.IsZero())
				currentAnimation = &idle;
			else
				currentAnimation = &run;

	}


	virtual fPoint GetShotDir(); 
	virtual fPoint GetShotSpeed(fPoint dir) const { return fPoint(dir.x * longRangeShootData.shotSpeed, dir.y * longRangeShootData.shotSpeed); }


	bool isWallBetweenPlayerAndMe(bool shoot = false); 


	void DebugPath() const
	{
		if (pathToFollow.size() > 0)
		{
			for (const auto& p : pathToFollow)
			{
				SDL_Rect r = { App->map->MapToWorld(p.x, 0).x,App->map->MapToWorld(0, p.y).y, 32, 32 }; 
				if (App->render->isRectOnCamera(r) == true)
				{
					SDL_Color c = { 0, 255, 0, 100 }; 

					if (lastSpeed.IsZero() == true)
						return; 

					App->render->DrawQuad(r, c.r, c.g, c.b, c.a, true, true);
				}
					
			}
				
		}
	}

public:

	ATTACK_TYPE currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE; 
	AttackData currentAttackData; 
	longShotData longRangeShootData;
	fxAndAnimData dataAnimFx;
	enemyPathType pathType; 
	baseDamageValues damageValues; 
	baseCadenceValues cadenceValues; 
	Animation run;
	Animation attack; 
	Animation death1;   
	Animation death2;
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0);
	TargetPos targetPos; 
	iPoint originTilePos = iPoint(0, 0); 
	fPoint deathPosGround = fPoint(0, 0);
	POINTING_DIR lastShotDir = POINTING_DIR::RIGHT; 
	iPoint specificDir = iPoint(0, 0); 
	uint myMeleeRange = 1U; 
	uint tileDetectionRange = 0U;
	uint maxDistFromOrigin = 0U;
	SDL_Rect lastPosCollider;
	bool playerNearby = false; 
	bool doJump = false;
	bool hasMaxDistanceFromOrigin = false; 
	bool playerInsideZone = false; 
	bool adaptativeColliderMovement = true; 
	bool resetAtoB = false; 
	bool stopLogic = false; 
	POINTING_DIR lastPointingDir; 
	ejumpData jumpInfo;
	j1EntityPlatform* lastPlatform = nullptr; 
	float platFormSpeed = 60.f;
	lastRaycastData lastRaycastInfo; 
	uint powerLevel = 1U; 

private: 
	float momentumFactor = 10.f;
	float momentum(float speed) { return speed * momentumFactor; };
	
	std::vector<iPoint> pathToFollow; 
	 
public:
	myEnemyState state;
	bool onDynamicplatform = false;
	

	friend class j1Entity;


};

#endif 
