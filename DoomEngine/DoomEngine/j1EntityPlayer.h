#ifndef _J1ENTITY_PLAYER_H__
#define _J1ENTITY_PLAYER_H__

#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1Render.h"
#include <array>



enum class combatState
{
	IDLE,
	SHOOT,
	DIE
};

enum class MovementState
{
	IDLE, 
	WALK,
	INPUT_RIGHT = 1,
	INPUT_LEFT = -1,
	JUMP,
	FALL
};

struct myState
{
	combatState combat; 
	std::array<MovementState, 2> movement; // 0 for idle, right. left, 1 for jump or fall

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

	bool IsAiming()
	{
		return aiming;
	}

private:

	bool aiming = false;
	myState state; 
	bool inputReady = true;
	
	float momentumFactor = 10.f; 
	float momentum(float speed) { return speed * momentumFactor; };

	// TODO: not only here, but in enemy cpp, add more anims, apart from idle





};

#endif _J1ENTITY_PLAYER_H__