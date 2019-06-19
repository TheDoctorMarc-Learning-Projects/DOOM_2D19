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

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->tex->Load("textures/player/player.png");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = PLAYER; 
	position = previousPosition = fPoint(posX, posY); 
	pointingDir = RIGHT;
	size.create(34, 53);
	speed = .0035f; 
	mass = 1.f; 
	gravityFactor = DEFAULT_GRAV * mass; 
	state.combat = combatState::IDLE;
	state.movement.at(0) = MovementState::IDLE; 
	state.movement.at(1) = MovementState::FALL;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({1, 421, size.x + 17, size.y}); 
	run.PushBack({ 8, 33, size.x + 8, size.y });
	run.PushBack({ 4, 130, size.x + 3, size.y + 2 });
	run.PushBack({ 7, 227, size.x + 4, size.y + 1});
	run.PushBack({ 7, 324, size.x, size.y + 1 });
	run.loop = true; 
	run.speed = 4.f; 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({(int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_PLAYER, this);


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
		lastSpeed = (xAxis * speed) * dt;
		position.x += lastSpeed;
		isMoving = true;
	
		//if(state.movement.at(0) == MovementState::IDLE)
			currentAnimation = &run;

		state.movement.at(0) = (xAxis < 0) ? MovementState::INPUT_LEFT : state.movement.at(0); 
		state.movement.at(0) = (xAxis > 0) ? MovementState::INPUT_RIGHT : state.movement.at(0);
		 
	}
	else
	{
		if (state.movement.at(0) != MovementState::IDLE)
		{
			state.movement.at(0) = MovementState::IDLE;
			//currentAnimation = &idle;
		}
	 
		currentAnimation = &idle;
	}
		

	/*for (int i = 0; i < 2; ++i)
	{

	


		  
	}*/

	// jump in the direction of joystick: 


	if(App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A))
		if (yAxis <= 0)
			if (onPlatform)
			{
				state.movement.at(1) = MovementState::JUMP; 


				onPlatform = false;
				ResetGravity();

			

			}
	
	if (!onPlatform)
		if(state.movement.at(1) == MovementState::FALL)
			position.y += GravityCalc(gravityFactor, mass) * dt;
		else if(state.movement.at(1) == MovementState::JUMP)
			position.y += (-(jumpInfo.currenJumpPower *= jumpInfo.jumpIncrementFactor)) + GravityCalc(gravityFactor, mass) * dt;
	

	if (position.y > previousPosition.y && state.movement.at(1) == MovementState::JUMP)
		state.movement.at(1) = MovementState::FALL;


	if (!to_delete)
		collider->SetPos(position.x, position.y);

	return true;
}

void j1EntityPlayer::OnCollision(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_WALL:
		if (state.movement.at(1) != MovementState::JUMP)
		{
			onPlatform = true;
			ResetGravity();

			state.movement.at(1) = MovementState::NOT_ACTIVE;   // jump or fall not active
		}
	
		break; 
	}

}


void j1EntityPlayer::OnCollisionExit(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_WALL:
		if (state.movement.at(1) != MovementState::JUMP)
		{
			onPlatform = false;
			ResetGravity();

			state.movement.at(1) = MovementState::FALL; 
		}
		break;
	}

}

POINTING_DIR j1EntityPlayer::GetDirection()
{

	if (lastSpeed < 0)
		return pointingDir = POINTING_DIR::LEFT;
	else if (lastSpeed > 0)
		return pointingDir = POINTING_DIR::RIGHT;
	
	return pointingDir;    
}



/*
bool j1EntityPlayer::InputCombat()
{
	

	// reset combat state
	combat_state = combatState::IDLE;
	

	return true;
}
*/



