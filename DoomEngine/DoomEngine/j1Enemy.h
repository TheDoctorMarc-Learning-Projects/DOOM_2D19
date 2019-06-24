#ifndef _J1ENEMY_H__
#define _J1ENEMY_H__

#include "j1Entity.h"
#include <vector> 

enum class eCombatState
{
	IDLE,
	SHOOT,
	DYING,
	DEAD
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
	bool CleanUp() override;

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void WarnOtherModules();
	void VerticalMovement(float dt); 
	bool FollowPlayer(float dt); 
	void SolveMove(fPoint direction, float dt); 
	void AssignDirectionWithSpeed(); 
	bool IsAiming()
	{
		return aiming;
	}

public:

	Animation run;
	Animation death1;   // TODO: if all enemies have 2 death anims, put the other also here
	bool aiming = false;
	float life; 
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0);
	fPoint targetPos = fPoint(0, 0);
	uint tileDetectionRange = 0; 
	SDL_Rect lastPosCollider;
	bool doJump = false;
	POINTING_DIR lastPointingDir; 
	ejumpData jumpInfo;
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
