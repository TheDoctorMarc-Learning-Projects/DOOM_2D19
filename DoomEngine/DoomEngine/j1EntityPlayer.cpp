#include "j1EntityPlayer.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "j1Scene.h"
#include <math.h>


j1EntityPlayer::j1EntityPlayer(int posX, int posY) : j1Entity(PLAYER, posX , posY, "player")
{
	
	entityTex = App->tex->Load("textures/player/player.png");
	type = PLAYER; 
	position = previousPosition = iPoint(posX, posY); 
	currentAnimation = &idle; 
	size.create(34, 53);
	speed = .0045f; 

	idle.PushBack({18, 421, size.x, size.y}); 



	collider = App->collision->AddCollider({position.x, position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_PLAYER, this);

	pointingDir = RIGHT; 
}

j1EntityPlayer::~j1EntityPlayer()
{
}

bool j1EntityPlayer::Start()
{

	return true;
}

bool j1EntityPlayer::PreUpdate()
{
	
	return true;
}

bool j1EntityPlayer::Update(float dt)
{

	Move(dt);
	return true;
}

bool j1EntityPlayer::PostUpdate()
{

	return true;
}

bool j1EntityPlayer::CleanUp()
{
	
	if (entityTex != nullptr)
		App->tex->UnLoad(entityTex);


	return true;
}

bool j1EntityPlayer::Load(pugi::xml_node &)
{
	return true;
}

bool j1EntityPlayer::Save(pugi::xml_node &) const
{
	return true;
}



bool j1EntityPlayer::Move(float dt)
{
	bool isMoving = false;

	previousPosition = position;


	Sint16 xAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);
	Sint16 yAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);

	if (xAxis > 0 || xAxis < 0)
	{
		position.x += (xAxis * speed) * dt; 
		isMoving = true;
	
		state.movement.at(0) = (xAxis < 0) ? MovementState::INPUT_LEFT : state.movement.at(0); 
		state.movement.at(0) = (xAxis > 0) ? MovementState::INPUT_RIGHT : state.movement.at(0);
	
	}
	else
		state.movement.at(0) = MovementState::IDLE;

	for (int i = 0; i < 2; ++i)
	{

		if (!onPlatform)
			position.y += GLOBALSPACE_GRAVITY; 


		  
	}





	if (!to_delete)
		collider->SetPos(position.x, position.y);

	return true;
}

void j1EntityPlayer::OnCollision(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_WALL:
		onPlatform = true; 
		break; 
	}

}

/*
bool j1EntityPlayer::InputCombat()
{
	

	// reset combat state
	combat_state = combatState::IDLE;
	

	return true;
}
*/



