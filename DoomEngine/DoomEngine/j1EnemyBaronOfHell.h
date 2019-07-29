#ifndef _J1ENEMYBARONOFHELL_H__
#define _J1ENEMYBARONOFHELL_H__

#include "j1Enemy.h"
#include <array>

class j1Enemy;

class j1EnemyBaronOfHell : public j1Enemy
{
public:
	j1EnemyBaronOfHell(int posX, int posY, std::string name);
	~j1EnemyBaronOfHell();

	// functionality ------
	void ResolvePathDeviation();
	bool LongRangeConditions(); 
	fPoint GetShotSpeed(fPoint dir) const override; 
	bool IsWallBetweenPlayerAndMe(); 

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 


private:
	
	std::array<int, 4> lastRaycast; 
	bool debug = false; 
	float defaultSpeed = 0.f; 

};

#endif 


