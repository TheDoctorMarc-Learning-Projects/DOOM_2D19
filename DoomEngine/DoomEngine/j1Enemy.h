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
	float jumpPower = 20.5f;
	float currenJumpPower = jumpPower;
	float jumpIncrementFactor = .9f;
	float speedXIncrementJump = 1.7f;
	float speedXIncrementFall = 1.2f;
	float verticalIncrementFactor = 1.02f;     // jumpincrement reduces jump power in time, but vertical increment counteracts it a bit 
};


struct baseCadenceValues
{
	uint melee = 0U; 
	uint longRange = 0U; 
};

struct deathData
{
	bool hasSecondDeathAnim = false; 
	bool hasSecondDeathFx = false; 
};

class j1Entity;

class j1Enemy : public j1Entity
{
public:
	j1Enemy(int posX, int posY);
	~j1Enemy();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void VerticalMovement(float dt); 
	bool FollowPath(float dt); 
	bool CheckPathState(iPoint pos, iPoint& target, bool& success); 
	void CallPathCreation(iPoint pos, iPoint target, bool& success); 
	void SolveMove(fPoint direction, float dt); 
	void AssignDirectionWithSpeed(); 
	virtual void ResolvePathDeviation() {};
	
	void SetPath(float dt, bool& moveSuccess); 
	bool HasArrivedToTarget(iPoint tilePos, iPoint targetTilePos); 
	void SetPreviousFrameData();
	void SetCollider();
	void DieLogic(float dt); 

	virtual void DoAttack(bool meleeRange) {};

	virtual void SetDyingState(bool brutal = false)
	{
		state.combat = eCombatState::DYING; 

		App->audio->StopSpecificFx(name + "Injured");   // so that death is audible 

		if (!brutal || deathDataAnimFx.hasSecondDeathAnim == false)   // check this out (for the ones that only have one death anim) 
		{
			currentAnimation = &death1;
			App->audio->PlayFx(this->name + "Death");  // TODO: if two deaths sounds, play one or another
		}
			
		else
		{
			currentAnimation = &death2;

			if(deathDataAnimFx.hasSecondDeathFx)
				App->audio->PlayFx(this->name + "Death2");  // TODO: if two deaths sounds, play one or another
			else
				App->audio->PlayFx(this->name + "Death");  // TODO: if two deaths sounds, play one or another
		}
			


	};

	virtual void CheckDeathFinished() override
	{
		if (currentAnimation->Finished() == true)
		{
			state.combat = eCombatState::DEAD;
			
			to_delete = true;   // TODO: create the corpse here 

		}
	}

	bool isPlayerOnMeleeRange(); 

public:

	int damage = INT_MAX; 
	deathData deathDataAnimFx; 
	enemyPathType pathType; 
	baseCadenceValues cadenceValues; 
	Animation run;
	Animation attack; 
	Animation death1;   
	Animation death2;
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0);
	TargetPos targetPos; 
	fPoint deathPosGround = fPoint(0, 0);
	fPoint currentTarget = fPoint(0, 0); 
	SDL_Rect deathColllider = { 0, 0, 0, 0 }; 
	iPoint specificDir = iPoint(0, 0); 
	uint tileDetectionRange = 0; 
	SDL_Rect lastPosCollider;
	bool doJump = false;
	POINTING_DIR lastPointingDir; 
	ejumpData jumpInfo;
	j1EntityPlatform* lastPlatform = nullptr; 
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
