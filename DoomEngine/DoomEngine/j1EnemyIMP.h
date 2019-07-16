#ifndef _J1ENEMYIMP_H__
#define _J1ENEMYIMP_H__

#include "j1Enemy.h"




class j1Enemy;

class j1EnemyIMP : public j1Enemy
{
public:
	j1EnemyIMP(int posX, int posY, std::string name);
	~j1EnemyIMP();

	// functionality ------

	void Jump(); 
	void ResolvePathDeviation(); 

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 


private: 
	//float jumpAngle = 45; 
	uint jumpTriggerTileRange = 2; 
	uint extraJumpOffset = 4; 
	float speedMultiplierRampageAttack = 1.5f; 
	float RampageDamageMultiplier = 3.f; 
	float defaultSpeed = 0.f; 

};

#endif 

