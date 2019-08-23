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
		pointingDir = POINTING_DIR::DOWN;
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
		name = "platform1";
	else if(level == SceneState::LEVEL2)
		name = "platform2"; 

		 
  
	entityTex = App->map->entityTextureMap.at(name);


	if (movementType == AXIS_Movement::HORIZONTAL)
	{
		AreaCollider = App->collision->AddCollider({ (int)position.x - areaExtraSides / 2,
	(int)position.y,
	(int)((float)size.x * spriteScale) + areaExtraSides,
	(int)((float)size.y * spriteScale)}, COLLIDER_TYPE::COLLIDER_WALL_DETECTION, this);
	}

}

j1EntityPlatformDynamic::~j1EntityPlatformDynamic()
{
}


bool j1EntityPlatformDynamic::Update(float dt)
{
	BROFILER_CATEGORY("Entity Platform Update", Profiler::Color::AntiqueWhite);

	if (stopMovement)
		return false; 

	lastPointingDir = pointingDir;
	previousPosition = position;  

	switch (movementType)
	{
	case AXIS_Movement::HORIZONTAL:
	
		if (pointingDir == POINTING_DIR::RIGHT)
		{
			position.x += speed * dt;
			lastSpeed.x = speed * dt;
		}
			
		else if (pointingDir == POINTING_DIR::LEFT)
		{
			position.x -= speed * dt;
			lastSpeed.x = -speed * dt;
		}
		
		SetAreaColPos(); 

		break; 


	case AXIS_Movement::VERTICAL:
		CheckPlatformSameLevel();


		
		if (pointingDir == POINTING_DIR::UP)
		{
			position.y -= speed * dt;
			lastSpeed.y = -speed * dt;
		}

		else if (pointingDir == POINTING_DIR::DOWN)
		{
			position.y += speed * dt;
			lastSpeed.y = speed * dt;
		}
			


		

		break;

	}
	if (!collider->onCollisionWithMe.empty())
		UpdateEntitiesOnTopPositions();

	collider->SetPos(position.x, position.y);

	return true;
}


void j1EntityPlatformDynamic::SetAreaColPos()
{
	AreaCollider->SetPos(position.x - areaExtraSides / 2, position.y ); 
}

void j1EntityPlatformDynamic::UpdateEntitiesOnTopPositions(bool justOfsset, float offset)
{
	
	for (auto& col : collider->onCollisionWithMe)
	{
		if (col->type == COLLIDER_PLAYER && App->entityFactory->playerAlive)
			if (App->entityFactory->player->godMode == true)
				continue; 

		if (col == collider)  // the horizontal area collider is detected as on collision with the normal one 
			continue; 

		if (col->type == COLLIDER_PLAYER || col->type == COLLIDER_ENEMY || col->type == COLLIDER_BLOOD)    
		{
			if (movementType == AXIS_Movement::VERTICAL)
			{
				if (pointingDir == POINTING_DIR::UP)
					col->callback->position.y -= speed * App->GetDt();

				else if (pointingDir == POINTING_DIR::DOWN)
					col->callback->position.y += speed * App->GetDt(); 

			}
			else if(movementType == AXIS_Movement::HORIZONTAL)
				if (col->type == COLLIDER_BLOOD)
				{
					if (pointingDir == POINTING_DIR::RIGHT)
						col->callback->position.x += speed * App->GetDt();
					
					else if (pointingDir == POINTING_DIR::LEFT)
						col->callback->position.x -= speed * App->GetDt();
					
				}
		

			col->SetPos(col->callback->position.x, col->callback->position.y);
		}

	
	}
		
			
			


			
}

void j1EntityPlatformDynamic::CheckPlatformSameLevel()
{
	if (stopMovement == true)
		return; 

	if (pointingDir == UP && lastPointingDir != DOWN)
	{
		if (collider->rect.y <= App->entityFactory->platFormLevelHeights[heightLevel + levelsUp])  // if has reached Y pos 
		{
			pointingDir = POINTING_DIR::DOWN;

			if (levelsDown == 0)
				stopMovement = true;
		}
			
	}
		

	if (pointingDir == DOWN && lastPointingDir != UP)
	{
		if (collider->rect.y >= App->entityFactory->platFormLevelHeights[heightLevel - levelsDown])  // if has reached Y pos 
		{
			pointingDir = POINTING_DIR::UP;

			if (levelsUp == 0)
				stopMovement = true; 
		}
			
    }
		

	lastPointingDir = pointingDir;	
			
}


void j1EntityPlatformDynamic::OnCollision(Collider* c1, Collider* c2)
{
	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL_DETECTION)
	{
		switch (movementType)
		{
		case AXIS_Movement::HORIZONTAL:

			if ((c1 == collider && c2 == AreaCollider) || (c2 == collider && c1 == AreaCollider))
				return;

			if (c1 == AreaCollider && c2->type == COLLIDER_WALL_DETECTION && c2->callback && c2->callback->type != ENTITY_DYNAMIC)  // if it encounters eg cacodemon wall detection
				return; 

			if (!endReached)
			{

				if (AreaCollider->rect.x + AreaCollider->rect.w >= c2->rect.x &&  pointingDir == POINTING_DIR::RIGHT && lastPointingDir != POINTING_DIR::LEFT)
				{
					float offset = AreaCollider->rect.x + AreaCollider->rect.w - c2->rect.x;
					position.x -= offset;
					pointingDir = POINTING_DIR::LEFT;


					endReached = true;
				}

				else if (AreaCollider->rect.x <= c2->rect.x + c2->rect.w && pointingDir == POINTING_DIR::LEFT && lastPointingDir != POINTING_DIR::RIGHT)
				{

					float offset = c2->rect.x + c2->rect.w - AreaCollider->rect.x;
					position.x += offset;
					pointingDir = POINTING_DIR::RIGHT;

					endReached = true;
				}

				lastPointingDir = pointingDir;

	
			}

			break;
		}
	}

	
	

}


void j1EntityPlatformDynamic::OnCollisionExit(Collider* c1, Collider* c2)
{

	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL_DETECTION)
	{
		switch (movementType)
		{
		case AXIS_Movement::HORIZONTAL:
			if (endReached)
				endReached = false;
			break;
		}

	}



	

}

bool j1EntityPlatformDynamic::CleanUp()
{
	j1Entity::CleanUp(); 

	if (movementType == AXIS_Movement::HORIZONTAL)
		AreaCollider->to_delete = true; 

	return true;
}


