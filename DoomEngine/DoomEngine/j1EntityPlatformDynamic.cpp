#include "j1EntityPlatformDynamic.h"
#include "j1Collision.h"
#include "j1Map.h"
#include "j1EntityPlayer.h"
#include "j1Map.h"
#include "j1EntityFactory.h"

j1EntityPlatformDynamic::j1EntityPlatformDynamic(SDL_Rect placing, int heightLevel, int levelsUp, int levelsDown, SceneState level, AXIS_Movement movementType) : j1EntityPlatform(placing, heightLevel)
{



	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENTITY_DYNAMIC; // update this in dynamic platforms
	speed = 100.f;
	
	this->movementType = movementType; 
	
	
	if(this->movementType == AXIS_Movement::HORIZONTAL)
		pointingDir = POINTING_DIR::LEFT;
	else if (this->movementType == AXIS_Movement::VERTICAL)
	{
		pointingDir = POINTING_DIR::UP;
		this->levelsUp = levelsUp;
		this->levelsDown = levelsDown; 
	}
	

	// (collider and heightlevel already loaded in parent) 
	section.x = 0; 
	section.y = 0; 
	section.w = placing.w;
	section.h = placing.h; 
	previousPosition = position;


	if (level == SceneState::LEVEL1)
		entityTex = App->map->platfTexture; 
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


	BROFILER_CATEGORY("Entity Platform Update", Profiler::Color::AntiqueWhite);

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


	case AXIS_Movement::VERTICAL:
		CheckPlatformSameLevel();

		if (pointingDir == POINTING_DIR::UP)
			position.y -= speed * dt;
		else if (pointingDir == POINTING_DIR::DOWN)
			position.y += speed * dt;


		if(!collider->onCollisionWithMe.empty())
			UpdateEntitiesOnTopPositions();
	

		break;

	}

	collider->SetPos(position.x, position.y);

	return true;
}


void j1EntityPlatformDynamic::UpdateEntitiesOnTopPositions(bool justOfsset, float offset)
{
	
	for (auto& col : collider->onCollisionWithMe)
	{
		if (col->type == COLLIDER_PLAYER || col->type == COLLIDER_ENEMY)  // god also?? 
		{
			if (pointingDir == POINTING_DIR::UP)
			{
				if (!justOfsset)
					col->callback->position.y -= speed * App->GetDt();
				else
					col->callback->position.y -= offset; 
				
			}
				
			else if (pointingDir == POINTING_DIR::DOWN)
			{
				if (!justOfsset)
					col->callback->position.y += speed * App->GetDt();
				else
					col->callback->position.y += offset;
			}
				


			col->SetPos(position.x, position.y);
		}

	
	}
		
			
			


			
}

void j1EntityPlatformDynamic::CheckPlatformSameLevel()
{
	for (auto& platf : App->entityFactory->entities)    // TODO: Rework this, its ugly code 
	{
		if (platf->type == ENTITY_TYPE::ENTITY_STATIC)
		{
			/*if (!endReached)
			{*/
				float offset = 0.f;
				if (pointingDir == UP && lastPointingDir != DOWN)
				{
					if (collider->rect.y <= platf->collider->rect.y)  // if has reached Y pos 
					{
						if (heightLevel + levelsUp == dynamic_cast<j1EntityPlatform*>(platf)->heightLevel) // if that platf has desired height level
						{

						//	offset = platf->collider->rect.y /*+ platf->collider->rect.h*/ - collider->rect.y;   // to put back player if it goes off a bit
						//	position.y += offset;
							pointingDir = POINTING_DIR::DOWN;

						//	UpdateEntitiesOnTopPositions(true, offset); 
						}
					}

				}
				else if (pointingDir == DOWN && lastPointingDir != UP)
				{
					if (collider->rect.y >= platf->collider->rect.y)
					{
						if (heightLevel - levelsDown == dynamic_cast<j1EntityPlatform*>(platf)->heightLevel) // if that platf has desired height level
						{

						/*	offset = collider->rect.y + collider->rect.h - platf->collider->rect.y;
							position.y -= offset;*/
							pointingDir = POINTING_DIR::UP;

						//	UpdateEntitiesOnTopPositions(true, offset); 
						}
					}

				}


				lastPointingDir = pointingDir;
				//endReached = true;
			//}
			
		}
	}
		
			
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


		case AXIS_Movement::VERTICAL:

			/*if (!endReached)
			{*/
				float offset = 0.f;

				if (!c2->hasCallback)   // collider with floor, ignore side platforms
				{
					if (collider->rect.y + collider->rect.h > c2->rect.y &&  pointingDir == POINTING_DIR::DOWN && lastPointingDir != POINTING_DIR::UP)
					{
						offset = collider->rect.y + collider->rect.h - c2->rect.y;
						position.y -= offset;
						pointingDir = POINTING_DIR::UP;

						UpdateEntitiesOnTopPositions(true, offset);
					}
				}
			

				/*else if (collider->rect.y < c2->rect.y + c2->rect.h && pointingDir == POINTING_DIR::UP && lastPointingDir != POINTING_DIR::DOWN)
				{

					offset = c2->rect.y + c2->rect.h - collider->rect.y;   // to put back player if it goes off a bit
					position.y += offset;
					pointingDir = POINTING_DIR::DOWN;
				}*/

				lastPointingDir = pointingDir;
			//	endReached = true;
			//}

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

		switch (movementType)
		{
		case AXIS_Movement::HORIZONTAL:
			if (endReached)
				endReached = false;
		}
		
		break;

	}


	

}

bool j1EntityPlatformDynamic::PostUpdate()
{

	return true;
}


