#ifndef _J1ENEMYBARONOFHELL_H__
#define _J1ENEMYBARONOFHELL_H__

#include "j1Enemy.h"

class j1Enemy;

class j1EnemyBaronOfHell : public j1Enemy
{
public:
	j1EnemyBaronOfHell(int posX, int posY, std::string name);
	~j1EnemyBaronOfHell();

	// functionality ------
	void ResolvePathDeviation();

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 
	bool Go_A_to_B(); 

private:

	float platFormSpeed = 60.f; 
	float defaultSpeed = 0.f; 

};

#endif 


