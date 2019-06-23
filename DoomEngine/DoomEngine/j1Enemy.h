#ifndef _J1ENEMY_H__
#define _J1ENEMY_H__



#include "j1Entity.h"


enum class eCombatState
{
	IDLE,
	SHOOT,
	DIE
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
	float jumpPower = 1.f;
	float currenJumpPower = jumpPower;
	float jumpIncrementFactor = 1.f;
	float speedXIncrementJump = 1.f;
	float speedXIncrementFall = 1.f;
	float verticalIncrementFactor = 1.f;    // jumpincrement reduces jump power in time, but vertical increment counteracts it a bit 
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
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void WarnOtherModules();
	void FollowPlayer(); 

	bool IsAiming()
	{
		return aiming;
	}

private:

	Animation run;
	bool aiming = false;
	fPoint lastSpeed = fPoint(0, 0);
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0);
	float momentumFactor = 10.f;
	float momentum(float speed) { return speed * momentumFactor; };
	SDL_Rect lastPosCollider;
	ejumpData jumpInfo;

public:
	myEnemyState state;
	bool onDynamicplatform = false;
	

	friend class j1Entity;


};

#endif 
