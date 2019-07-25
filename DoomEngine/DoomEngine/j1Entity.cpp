#include "j1App.h"
#include "j1Entity.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "p2Log.h"
#include "Brofiler\Brofiler.h"
#include "j1PathFinding.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1EntityPlayer.h"
#include "j1EnemyIMP.h"


j1Entity::j1Entity(ENTITY_TYPE type, float positionX, float positionY,std::string name) : type(type), position(positionX,positionY), name(name)
{


	this->name = name; 
}


j1Entity::j1Entity(ENTITY_TYPE type, std::string name) : type(type), name(name)
{
}



j1Entity::j1Entity(float positionX, float positionY) : position(positionX, positionY)
{
}

// TODO, not here, but in each individual entity, do a size.create with the collider dimensions, it will be used in the entity factory PostUpdate Draw() calls


j1Entity::~j1Entity()
{
}

bool j1Entity::Start()
{
	return true;
}

bool j1Entity::PreUpdate()
{
	return true;
}

bool j1Entity::Update(float dt)
{
	

	/*if (!to_delete)
		collider->SetPos(position.x, position.y); */


	return true;
}
bool j1Entity::PostUpdate()
{

	return true;
}

bool j1Entity::CleanUp()
{
	if(colliderActive == true)
		collider->to_delete = true;

	colliderActive = false; 

	/*if (entityTex != nullptr)    // loaded and unloaded in atlas 
	{
		App->tex->UnLoad(entityTex);
	}*/

	return true;
}

void j1Entity::Draw()
{
	BROFILER_CATEGORY("Entity Draw", Profiler::Color::Aqua);

	if (useRenderFlip)
	{
		if (GetDirection() == defaultPointingDir)
			flip = SDL_FLIP_NONE;
		else
			flip = SDL_FLIP_HORIZONTAL;

	}
		

	if (drawActive && entityTex != nullptr)
	{
		if (section.w > 0 && section.h > 0)  // easy way to distinguish basic loot and stuff
		{
			if(currentAnimation == nullptr)
				App->render->Blit(entityTex, position.x, position.y, &section, 1.f, flip, spriteScale, spriteRotation, rotationPivots.x, rotationPivots.y);
			else
				App->render->Blit(entityTex, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.f, flip, spriteScale, spriteRotation, rotationPivots.x, rotationPivots.y);
		}
			
		else if(currentAnimation)
			App->render->Blit(entityTex, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.f, flip, spriteScale, spriteRotation, rotationPivots.x, rotationPivots.y);
		
	}
		
	
		

}

POINTING_DIR j1Entity::GetDirection()
{

 
		if (lastSpeed.x < 0)
			return pointingDir = POINTING_DIR::LEFT;
		else if (lastSpeed.x > 0)
			return pointingDir = POINTING_DIR::RIGHT;

		return pointingDir;    // no change in speed results in same pointing dir

}

bool j1Entity::Move(float dt)
{
	return true;
}


void j1Entity::ResetGravity()
{
	
	gravityFactor = DEFAULT_GRAV / mass;

	if (type == ENTITY_TYPE::PLAYER)
		dynamic_cast<j1EntityPlayer*>(this)->jumpInfo.currenJumpPower = dynamic_cast<j1EntityPlayer*>(this)->jumpInfo.jumpPower;
	else if(type == ENTITY_TYPE::ENEMY_IMP)
		dynamic_cast<j1EnemyIMP*>(this)->jumpInfo.currenJumpPower = dynamic_cast<j1EnemyIMP*>(this)->jumpInfo.jumpPower;

}

