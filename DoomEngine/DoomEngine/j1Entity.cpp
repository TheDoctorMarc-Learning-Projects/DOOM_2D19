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

	ID = App->entityFactory->NumberOfSpawns; 
}


j1Entity::j1Entity(ENTITY_TYPE type, std::string name) : type(type), name(name)
{

	ID = App->entityFactory->NumberOfSpawns;
}



j1Entity::j1Entity(float positionX, float positionY) : position(positionX, positionY)
{

	ID = App->entityFactory->NumberOfSpawns;
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


// - - - - - - - - - - - - - - - - - - - - - - all entities have common variables to already be save and loaded here

bool j1Entity::Load(pugi::xml_node& node)  // the node is the entitynode (captured for entities that I want with a special name (eg blood)) 
{

	// POSITION, SPEED, DIRECTION
	position.x = node.child("position").attribute("x").as_float();
	position.y = node.child("position").attribute("y").as_float();

	previousPosition.x = node.child("previous_position").attribute("x").as_float();
	previousPosition.y = node.child("previous_position").attribute("y").as_float();

	std::string directionString = node.child("direction").attribute("value").as_string();
	 
	if (directionString == "right")
		pointingDir = RIGHT; 
	else if (directionString == "left")
		pointingDir = LEFT;
	else if (directionString == "up")
		pointingDir = UP;
	else if (directionString == "down")
		pointingDir = DOWN;

	lastSpeed.x = node.child("last_speed").attribute("x").as_float();
	lastSpeed.y = node.child("last_speed").attribute("y").as_float();

	// BOOLS
	colliderActive = node.child("collider_active").attribute("value").as_bool();

	if (colliderActive == false)
		App->collision->DestroyCollider(collider); 

	to_delete = node.child("to_delete").attribute("value").as_bool();

	return true;
}


bool j1Entity::Save(pugi::xml_node& node) const  // it recieves the netity node and fills the data
{
	entityNode = node;

	// POSITION, SPEED, DIRECTION
	auto n = entityNode.append_child("name"); 
	n.append_attribute("value") = name.c_str();

	auto identification = entityNode.append_child("ID");
	identification.append_attribute("value") = ID;

	auto pos = entityNode.append_child("position");
	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y; 

	auto previouspos = entityNode.append_child("previous_position");
	previouspos.append_attribute("x") = previousPosition.x;
	previouspos.append_attribute("y") = previousPosition.y;

	auto direction = entityNode.append_child("direction");

	switch (pointingDir)
	{
	case RIGHT:
		direction.append_attribute("value") = "right";
		break;
	case LEFT:
		direction.append_attribute("value") = "left";
		break;
	case UP:
		direction.append_attribute("value") = "up";
		break;
	case DOWN:
		direction.append_attribute("value") = "down";
		break;
	}
	
	auto lSpeed = entityNode.append_child("last_speed");
	lSpeed.append_attribute("x") = lastSpeed.x;
	lSpeed.append_attribute("y") = lastSpeed.y;

	// BOOLS
	auto colActive = entityNode.append_child("collider_active");
	colActive.append_attribute("value") = colliderActive; 

	auto del = entityNode.append_child("to_delete");
	del.append_attribute("value") = to_delete;

	return true;
}


