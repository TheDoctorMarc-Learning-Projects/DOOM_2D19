#ifndef _J1ENEMYHELLKNIGHT_H__
#define _J1ENEMYHELLKNIGHT_H__

#include "j1Enemy.h"

class j1Enemy;

enum class BEHAVIOUR_STATE
{
   A_TO_B, // TODO: at the beginning, and if player falls to base floor? 
   FOLLOW,
};

class j1EnemyHellKnight : public j1Enemy
{
public:
	j1EnemyHellKnight(int posX, int posY, std::string name);
	~j1EnemyHellKnight();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// functionality ------
	void ResolvePathDeviation();
	void ChangeState(); 
	void FallToPlayer(); 

	bool Move(float dt) override;  // call parent function, then if enemy does enaything extra, do it after call 


private:

	BEHAVIOUR_STATE myState = BEHAVIOUR_STATE::A_TO_B; 
	float platFormSpeed = 60.f;
	float defaultSpeed = 0.f;
	j1EntityPlatform* targetPlatform = nullptr; 
};

#endif 



