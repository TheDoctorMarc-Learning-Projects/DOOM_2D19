#ifndef _J1ENEMY_H__
#define _J1ENEMY_H__

#include "j1Entity.h"
#include "j1EntityPlatform.h"
#include <vector> 

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
	LOOP_DEVIATION
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
	//bool CleanUp();  // not yey, only is we have an extra texture or something

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void WarnOtherModules();
	void VerticalMovement(float dt); 
	bool FollowPath(float dt); 
	void SolveMove(fPoint direction, float dt); 
	void AssignDirectionWithSpeed(); 
	virtual void ResolvePathDeviation() {};


	virtual void DoAttack(bool meleeRange) {};
	bool isPlayerOnMeleeRange(); 

public:

	int damage = INT_MAX; 
	baseCadenceValues cadenceValues; 
	Animation run;
	Animation attack; 
	Animation death1;   // TODO: if all enemies have 2 death anims, put the other also here
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0);
	TargetPos targetPos; 
	fPoint deathPosGround = fPoint(0, 0);
	fPoint currentTarget = fPoint(0, 0); 
	SDL_Rect deathColllider = { 0, 0, 0, 0 }; 
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
