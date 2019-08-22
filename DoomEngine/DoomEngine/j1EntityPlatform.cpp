#include "j1EntityPlatform.h"
#include "j1EntityFactory.h"
#include "j1Collision.h"
#include "j1Map.h"

#include <math.h>


j1EntityPlatform::j1EntityPlatform(SDL_Rect placing, int heightLevel) : j1Entity(PLAYER, "platform")
{

	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENTITY_STATIC; // update this in dynamic platforms
	this->heightLevel = heightLevel; 

	position.x = placing.x; 
	position.y = placing.y; 
	size.create(placing.w, placing.h); 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ placing.x, placing.y, placing.w, placing.h }, COLLIDER_TYPE::COLLIDER_FLOOR, this);

	 
}

