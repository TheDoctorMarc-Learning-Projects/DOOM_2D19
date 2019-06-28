#ifndef _J1ENTITY_PLAYER_H__
#define _J1ENTITY_PLAYER_H__


#include "j1Entity.h"
#include "j1EntityPlatform.h"
#include "j1EntityLoot.h"
#include "j1EntityLootWeapon.h"
#include <vector>

//#define jumpPower 20.f

#define jumpComfortCornerThreshold 20

enum class combatState
{
	IDLE,
	AIM,
	STUN,
	DYING,
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
	std::array<MovementState, 2> movement; // 0 for idle, run, stun, 1 for jump or fall

};

struct jumpData
{
	float jumpPower = 16.5f; 
	float currenJumpPower = jumpPower; 
	float jumpIncrementFactor = .9f;
	float speedXIncrementJump = 1.7f; 
	float speedXIncrementFall = 1.2f;
	float verticalIncrementFactor = 1.02f;    // jumpincrement reduces jump power in time, but vertical increment counteracts it a bit 
};

class j1Entity; 

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
	bool CleanUp() override;

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void HorizonatlMovement(float dt); 
	void VerticalMovement(float dt);
	void SetPreviousFrameData(); 
	void SetCollider(); 
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void WarnOtherModules();
	void WeaponLogic(); 
	void ChangeWeapon(SDL_GameControllerButton button);
	void AimWeapon(); 
	void ShootWeapon(); 


	POINTING_DIR GetDirection() override; 

	bool IsAiming()
	{
		return aiming;
	}

private:

	Animation run; 
	Animation aimUp; 
	Animation aimDown; 
	bool aiming = false;
	fPoint lastGroundPos = fPoint(0, 0);
	fPoint lastAirPos = fPoint(0, 0); 


	jumpData jumpInfo; 
	float momentumFactor = 10.f; 
	float momentum(float speed) { return speed * momentumFactor; };
	

	
	bool godMode = false; 
public: 
	myState state;
	bool onDynamicplatform = false;
	SDL_Rect lastPosCollider;
	j1EntityPlatform* lastPlatform = nullptr;
	j1EntityLootWeapon* currentWeapon = nullptr; 

	std::vector<j1EntityLootWeapon*> myWeapons; 


	friend class j1Entity; 


};

#endif 