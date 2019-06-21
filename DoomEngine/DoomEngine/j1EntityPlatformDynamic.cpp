#include "j1EntityPlatformDynamic.h"
#include "j1Collision.h"
#include "j1Map.h"
#include <math.h>


j1EntityPlatformDynamic::j1EntityPlatformDynamic(SDL_Rect placing, int heightLevel, int heightDeltaMovement, SceneState level, AXIS_Movement movementType) : j1EntityPlatform(placing, heightLevel)
{



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENTITY_DYNAMIC; // update this in dynamic platforms
	
	// (collider and heightlevel already loaded in parent) 
	section.x = 0; 
	section.y = 0; 
	section.w = placing.w;
	section.h = placing.h; 
	previousPosition = position;

	if (level == SceneState::LEVEL1)
		entityTex = App->tex->Load("maps/textures/plat1.png");
	/*else if (level == SceneState::LEVEL2)
		entityTex = App->tex->Load(); */

}

j1EntityPlatformDynamic::~j1EntityPlatformDynamic()
{
}

bool j1EntityPlatformDynamic::Start()
{

	return true;
}

bool j1EntityPlatformDynamic::PreUpdate()
{

	return true;
}

bool j1EntityPlatformDynamic::Update(float dt)
{


	return true;
}

bool j1EntityPlatformDynamic::PostUpdate()
{

	return true;
}

bool j1EntityPlatformDynamic::CleanUp()
{
	

	if (entityTex)
		App->tex->UnLoad(entityTex); 
	

	return true;
}


