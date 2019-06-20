#include "j1EntityPlatform.h"
#include "j1EntityFactory.h"
#include "j1Collision.h"
#include "j1Map.h"

#include <math.h>


j1EntityPlatform::j1EntityPlatform(SDL_Rect placing, int heightLevel) : j1Entity(PLAYER, "platform")
{

	

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENTITY_STATIC; // update this in dynamic platforms
	


	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ placing.x - App->map->data.tile_width, placing.y - App->map->data.tile_height, placing.w, placing.h }, COLLIDER_TYPE::COLLIDER_FLOOR, this);

	 
}

j1EntityPlatform::~j1EntityPlatform()
{
}

bool j1EntityPlatform::Start()
{

	return true;
}

bool j1EntityPlatform::PreUpdate()
{

	return true;
}

bool j1EntityPlatform::Update(float dt)
{

	
	return true;
}

bool j1EntityPlatform::PostUpdate()
{

	return true;
}

bool j1EntityPlatform::CleanUp()
{
	collider->to_delete = true;


	return true;
}

