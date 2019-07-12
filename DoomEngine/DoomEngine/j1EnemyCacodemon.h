#ifndef _J1ENEMYCACODEMON_H__
#define _J1ENEMYCACODEMON_H__

#include "j1Enemy.h"

enum class CACODEMON_ATTACK_TYPE
{
	MELEE,
	FIREBALL,
	NO_ATTACK_TYPE
};

struct actualCollisionType
{
	bool top = false; 
	bool bottom = false; 
	bool right = false; 
	bool left = true; 
};

class j1Enemy;

class j1EnemyCacodemon : public j1Enemy
{
public:
	j1EnemyCacodemon(int posX, int posY, std::string name);
	~j1EnemyCacodemon();

	// functionality ------

	//void ResolvePathDeviation();

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 

	void OnCollision(Collider* c1, Collider* c2) override;        // TODO: cacodemon ignores jump, fall, onplatform etc
    void OnCollisionExit(Collider* c1, Collider* c2) override;    

	bool CleanUp() override; 
	void SetDeviation(bool horizontal, Collider* c2);
	void KeepMovingTendency(); 

	void ResetPlatformState() {
		onPlatFormType.bottom = false; 
		onPlatFormType.left = false; 
		onPlatFormType.right = false; 
		onPlatFormType.top = false; 
	}


	iPoint GetShieldPos() { return iPoint(position.x - shieldExtraSideSize / 2, position.y - shieldExtraSideSize / 2); };
	iPoint GetLastShieldPos() { return iPoint(lastAirPos.x - shieldExtraSideSize / 2, lastAirPos.y - shieldExtraSideSize / 2); };

	SDL_Rect GetShieldRect() {
		return { collider->rect.x - shieldExtraSideSize / 2, collider->rect.y - shieldExtraSideSize / 2,
				 collider->rect.w + shieldExtraSideSize, collider->rect.h + shieldExtraSideSize };
	};
	SDL_Rect GetLastShieldRect() {
		return { lastPosCollider.x - shieldExtraSideSize / 2, lastPosCollider.y - shieldExtraSideSize / 2,
                 lastPosCollider.w + shieldExtraSideSize, lastPosCollider.h + shieldExtraSideSize }; 
	}; 

private:
	int shieldExtraSideSize = 30; 
	bool keepMovingAfterPlatform = false; 
	actualCollisionType onPlatFormType; 
	uint stopNearPlayerRange = 10; 
	CACODEMON_ATTACK_TYPE currentAttackType = CACODEMON_ATTACK_TYPE::NO_ATTACK_TYPE;
	Collider* shieldAreaCollider = nullptr; 
};

#endif 


