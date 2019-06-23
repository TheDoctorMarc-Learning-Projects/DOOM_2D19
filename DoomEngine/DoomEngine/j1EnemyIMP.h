#ifndef _J1ENEMYIMP_H__
#define _J1ENEMYIMP_H__

#include "j1Enemy.h"



class j1Enemy;

class j1EnemyIMP : public j1Enemy
{
public:
	j1EnemyIMP(int posX, int posY);
	~j1EnemyIMP();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// functionality ------
	//bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 


};

#endif 

