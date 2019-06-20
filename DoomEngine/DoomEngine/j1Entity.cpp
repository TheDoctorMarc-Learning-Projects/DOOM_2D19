#include "j1App.h"
#include "j1Entity.h"
#include "j1Render.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "p2Log.h"
#include "Brofiler\Brofiler.h"
#include "j1PathFinding.h"
#include "j1Map.h"
#include "j1Collision.h"
#include "j1EntityPlayer.h"

j1Entity::j1Entity(ENTITY_TYPE type, float positionX, float positionY,std::string name) : type(type), position(positionX,positionY), name(name)
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

	/*if (entityTex != nullptr)
		App->tex->UnLoad(entityTex); */

	return true;
}

void j1Entity::Draw()
{
	
	if (GetDirection() == POINTING_DIR::LEFT)
		flip = SDL_FLIP_NONE;
	else
		flip = SDL_FLIP_HORIZONTAL; 


	//AdjustColliderToAnimFrame(); 

	if (entityTex != nullptr) 
		App->render->Blit(entityTex, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.f, flip, spriteScale);
	else 
		App->render->Blit(App->entityFactory->atlasTexture, position.x, position.y, &currentAnimation->GetCurrentFrame(), 1.f, flip, spriteScale);

		

}

fPoint j1Entity::GetPosition()
{
	return position;
}

POINTING_DIR j1Entity::GetDirection()
{

	// TODO: only do this with player and enemies  AND REWORK IT 






	if (speed < 0)
		return pointingDir = POINTING_DIR::LEFT; 
	else if(speed > 0)
		return pointingDir = POINTING_DIR::RIGHT;

	return pointingDir;    // no change in speed results in same pointing dir
}

bool j1Entity::Move(float dt)
{
	return true;
}


void j1Entity::ResetGravity()
{

	gravityFactor = DEFAULT_GRAV * mass;

	if (type == ENTITY_TYPE::PLAYER)
		dynamic_cast<j1EntityPlayer*>(this)->jumpInfo.currenJumpPower = dynamic_cast<j1EntityPlayer*>(this)->jumpInfo.jumpPower;
}

