#ifndef __PLAYERENTITY_H__
#define __PLAYERENTITY_H__

#include "p2Point.h"
#include "p2Animation.h"
#include "j1Entity.h"
#include "j1Render.h"


struct SDL_Texture;


enum class combatState
{
	IDLE,
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
   // bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;
	// functionality ------
	bool InputMovement(float dt);
	bool InputCombat();

	

	bool IsAiming()
	{
		return aiming;
	}

private:

	bool aiming = false;
	combatState combat_state; 
	bool inputReady = true;
	

	// TODO: not only here, but in enemy cpp, add more anims, apart from idle





};

#endif