#ifndef _J1ENTITY_PLATFORM_H__
#define _J1ENTITY_PLATFORM_H__

#include "j1Entity.h"


class j1Entity; 

class j1EntityPlatform : public j1Entity
{
public:
	j1EntityPlatform(SDL_Rect placing, int heightLevel);
	~j1EntityPlatform() {};

public:
	int heightLevel = 0;
}; 

#endif 