#ifndef _J1ENEMYCACODEMON_H__
#define _J1ENEMYCACODEMON_H__

#include "j1Enemy.h"

enum class CACODEMON_ATTACK_TYPE
{
	MELEE,
	FIREBALL,
	NO_ATTACK_TYPE
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

	//void OnCollision(Collider* c1, Collider* c2) override;        // TODO: cacodemon ignores jump, fall, onplatform etc
	//void OnCollisionExit(Collider* c1, Collider* c2) override;    

private:
	uint stopNearPlayerRange = 10; 

	CACODEMON_ATTACK_TYPE currentAttackType = CACODEMON_ATTACK_TYPE::NO_ATTACK_TYPE;
};

#endif 


