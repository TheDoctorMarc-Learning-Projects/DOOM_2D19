#ifndef _J1ENTITY_PLATFORM_H__
#define _J1ENTITY_PLATFORM_H__

#include "j1Entity.h"

struct collider;

class j1Entity; 

class j1EntityPlatform : public j1Entity
{
public:
	j1EntityPlatform(SDL_Rect placing, int heightLevel);
	~j1EntityPlatform();

	//core loops ------
	//bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();


	// todo, movable platforms load and save (Xd) 

public:
	int heightLevel = 0;
}; 

#endif _J1ENTITY_PLATFORM_H__