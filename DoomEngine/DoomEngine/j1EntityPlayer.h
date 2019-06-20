#ifndef _J1ENTITY_PLAYER_H__
#define _J1ENTITY_PLAYER_H__

#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1Render.h"
#include <array>

//#define jumpPower 20.f

#define jumpColliderThreshold 20

enum class combatState
{
	IDLE,
	SHOOT,
	DIE
};

enum class MovementState
{
	IDLE, 
	RUN,
	INPUT_RIGHT,
	INPUT_LEFT,
	JUMP,
	FALL,
	NOT_ACTIVE
};

struct myState
{
	combatState combat; 
	std::array<MovementState, 2> movement; // 0 for idle, run, 1 for jump or fall

};

struct jumpData
{
	float jumpPower = 16.f; 
	float currenJumpPower = jumpPower; 
	float jumpIncrementFactor = .88f; 
};

struct SpeedBlocked
{
	bool RX = false; 
	bool LX = false;
	bool RY = false; 
	bool LY = false; 
};

class j1EntityPlayer : public j1Entity
{
public:
	j1EntityPlayer(int posX, int posY);
	~j1EntityPlayer();

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

	POINTING_DIR GetDirection() override; 

	bool IsAiming()
	{
		return aiming;
	}

private:

	Animation run; 
	bool aiming = false;
	myState state; 
	fPoint lastSpeed = fPoint(0,0);
	SpeedBlocked speedBlocked; 
	jumpData jumpInfo; 
	float momentumFactor = 10.f; 
	float momentum(float speed) { return speed * momentumFactor; };
	SDL_Rect lastPosCollider; 

	// TODO: not only here, but in enemy cpp, add more anims, apart from idle


	friend class j1Entity; 


};

#endif _J1ENTITY_PLAYER_H__