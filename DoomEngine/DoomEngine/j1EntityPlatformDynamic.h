#ifndef _J1ENTITYPLATFORMDYNAMIC_H__
#define _J1ENTITYPLATFORMDYNAMIC_H__

#include "j1EntityPlatform.h"
#include "j1Scene.h"

enum AXIS_Movement
{
	VERTICAL,
	HORIZONTAL,
	DIAGONAL
};

class j1EntityPlatformDynamic : public j1EntityPlatform
{
public:
	j1EntityPlatformDynamic(SDL_Rect placing, int heightLevel, int heightDeltaMovement, SceneState level, AXIS_Movement movementType);
	~j1EntityPlatformDynamic();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();


	// todo, movable platforms load and save (Xd) 

public:
	AXIS_Movement movementType; 
};

#endif 
