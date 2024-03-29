#ifndef _J1ENTITY_PLAYER_H__
#define _J1ENTITY_PLAYER_H__


#include "j1Entity.h"
#include "j1EntityPlatform.h"
#include "j1EntityLoot.h"
#include "j1EntityLootWeapon.h"
#include "j1Input.h"
#include <vector>

//#define jumpPower 20.f

#define jumpComfortCornerThreshold 20
#define comfortShootAngle 45

enum class combatState
{
	IDLE,
	AIM,
	STUN,
	DYING,
	DEAD
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
	float jumpPower = 16.4f; 
	float currenJumpPower = jumpPower; 
	float jumpIncrementFactor = .9f;
	float speedXIncrementJump = 1.03f;
	float initialSpeedXIncrementJump = 1.03f; 
	float speedXIncrementFall = 1.2f;
	float verticalIncrementFactor = 1.005f;    // jumpincrement reduces jump power in time, but vertical increment counteracts it a bit 
};

class j1Entity; 

class j1EntityPlayer : public j1Entity
{
public:
	j1EntityPlayer(int posX, int posY, std::string name);
	~j1EntityPlayer();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool CleanUp() override;

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool Move(float dt) override;
	void HorizonatlMovement(float dt); 
	void VerticalMovement(float dt);
	void VerticalMovementGodMode(float dt);
	bool SolveMoveLimits();   // prevent bullshit when in god mode
	void SetPreviousFrameData(); 
	void SetCollider(); 
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void WarnOtherModules();
	void PickWeapon(j1EntityLootWeapon* callback, bool saveLoad = false);
	void WeaponLogic(); 
	void ChangeWeapon(SDL_GameControllerButton button);
	void AimWeapon(); 
	void ShootWeapon(j1KeyState state);
	void DieLogic(float dt); 
	
	void SetDyingState(bool brutal = false); 

	virtual void CheckDeathFinished() override
	{
		if (currentAnimation->Finished())   // assumes current anim is death, maybe check it... xd
		{
			state.combat = combatState::DEAD;
			to_delete = true;

		}
	}

	POINTING_DIR GetDirection() override; 

	bool IsAiming()	const {	return aiming;	}
	bool isParalized()	const { return isnan(paralizedDir) == false;	}
	void UnParalize() { paralizedDir = std::numeric_limits<double>::quiet_NaN();  }

public:

	Animation run; 
	Animation aimUp; 
	Animation aimDown; 
	Animation death1;
	Animation death2;

private: 
	bool aiming = false;
	fPoint deathPosGround = fPoint(0, 0);
	double paralizedDir = std::numeric_limits<double>::quiet_NaN();   // when colliding horizontally with enemy, player can't go in that dir (it can be 1, -1 or nan)

	jumpData jumpInfo; 
	float momentumFactor = 10.f; 
	float momentum(float speed) { return speed * momentumFactor; };
	
public: 
	bool godMode = false;
	float armor = 0.0f; 
	float maxArmor = 0.0f; 
	myState state;
	bool onDynamicplatform = false;
	SDL_Rect lastPosCollider;
	j1EntityPlatform* lastPlatform = nullptr;
	j1EntityLootWeapon* currentWeapon = nullptr; 

	std::vector<j1EntityLootWeapon*> myWeapons; 


	friend class j1Entity; 


};

#endif 