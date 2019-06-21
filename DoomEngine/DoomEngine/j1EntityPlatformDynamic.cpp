#include "j1EntityPlatformDynamic.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1EntityPlayer.h"



j1EntityPlatformDynamic::j1EntityPlatformDynamic(SDL_Rect placing, int heightLevel, int heightDeltaMovement, SceneState level, AXIS_Movement movementType) : j1EntityPlatform(placing, heightLevel)
{



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENTITY_DYNAMIC; // update this in dynamic platforms
	speed = 100.f;
	pointingDir = POINTING_DIR::LEFT; 
	this->movementType = movementType; 
	
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
	lastPointingDir = pointingDir;
	previousPosition = position; 

	switch (movementType)
	{
	case AXIS_Movement::HORIZONTAL:

		if (pointingDir == POINTING_DIR::RIGHT)
			position.x += speed * dt; 
		else if(pointingDir == POINTING_DIR::LEFT)
			position.x -= speed * dt;

		break; 
	}

	collider->SetPos(position.x, position.y);

	return true;
}


void j1EntityPlatformDynamic::OnCollision(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:

		switch (movementType)
		{
		case AXIS_Movement::HORIZONTAL:

			

			if (!endReached)
			{
				float offset = 0.f;

				if (collider->rect.x + collider->rect.w >= c2->rect.x &&  pointingDir == POINTING_DIR::RIGHT && lastPointingDir != POINTING_DIR::LEFT)
				{
					offset = collider->rect.x + collider->rect.w - c2->rect.x;
					position.x -= offset;
					pointingDir = POINTING_DIR::LEFT;
				}

				else if (collider->rect.x <= c2->rect.x + c2->rect.w && pointingDir == POINTING_DIR::LEFT && lastPointingDir != POINTING_DIR::RIGHT)
				{

					offset = c2->rect.x + c2->rect.w - collider->rect.x;
					position.x += offset;
					pointingDir = POINTING_DIR::RIGHT;
				}

				lastPointingDir = pointingDir;
				endReached = true; 
			}
			
			

			break;
		}

		break;
	}

	

}


void j1EntityPlatformDynamic::OnCollisionExit(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:

		if (endReached)
			endReached = false;
		
		
		break;

	}


	

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


