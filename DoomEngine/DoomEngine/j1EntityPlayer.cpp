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
	entityTex = App->entityFactory->playerTexture; 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = PLAYER; 
	position = previousPosition = fPoint(posX, posY); 
	pointingDir = RIGHT;
	size.create(34, 53);
	speed = .0055f; 
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
	run.speed = 2.f; 

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

	return true;
}

bool j1EntityPlayer::PostUpdate()
{

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

	previousPosition = position;

	if (onPlatform)
		lastGroundPos = position;
	else
		lastAirPos = position;


	// dirty stuff just to debug for the mom 

	if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		godMode = !godMode; 
	
	if (godMode)
		jumpInfo.jumpPower = 40.f;
	else
		jumpInfo.jumpPower = 17.f; 


	if (!onPlatform)
		onDynamicplatform = false; 

	lastPosCollider = collider->rect; 

	Sint16 xAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);
	Sint16 yAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTY);

	// - - - - - - - - - - - - - - - - - - horizontal movement
	if (xAxis > 0 || xAxis < 0)
	{


		currentAnimation = &run;

		if (state.movement.at(1) != MovementState::NOT_ACTIVE)
		{
			if(state.movement.at(1) == MovementState::JUMP)
				lastSpeed.x = (xAxis * speed) * jumpInfo.speedXIncrementJump* dt;
			else if(state.movement.at(1) == MovementState::FALL)
				lastSpeed.x = (xAxis * speed) * jumpInfo.speedXIncrementFall* dt;
			
		}
		else
			lastSpeed.x = (xAxis * speed) * dt;
			
		state.movement.at(0) = (xAxis < 0) ? MovementState::INPUT_LEFT : state.movement.at(0);
		state.movement.at(0) = (xAxis > 0) ? MovementState::INPUT_RIGHT : state.movement.at(0); 

		position.x += lastSpeed.x;

	}
	else
	{
		if (state.movement.at(0) != MovementState::IDLE)
		{
			state.movement.at(0) = MovementState::IDLE;
			//currentAnimation = &idle;

		}
	 
		currentAnimation = &idle;
		lastSpeed.x = 0; 
	}
 

	// - - - - - - - - - - - - - - - - - - vertical movement

	if(App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN)
		if (yAxis <= 0)
			if (onPlatform)
			{
				state.movement.at(1) = MovementState::JUMP; 
				App->audio->PlayFx("dash");


				onPlatform = false;
				ResetGravity();

			

			}
	
	if (!onPlatform)
	{
		if (state.movement.at(1) == MovementState::FALL)
		{
			lastSpeed.y = GravityCalc(gravityFactor, mass) * dt;
			position.y += lastSpeed.y;
		}

		else if (state.movement.at(1) == MovementState::JUMP)
		{
			if(lastSpeed.x == 0)
				lastSpeed.y = (-(jumpInfo.currenJumpPower *= jumpInfo.jumpIncrementFactor * jumpInfo.verticalIncrementFactor)) + GravityCalc(gravityFactor, mass) * dt;

			else 
				lastSpeed.y = (-(jumpInfo.currenJumpPower *= jumpInfo.jumpIncrementFactor)) + GravityCalc(gravityFactor, mass) * dt;

			position.y += lastSpeed.y;

			if (lastSpeed.y > 0)
				state.movement.at(1) = MovementState::FALL;

		}

	}
	else
		lastSpeed.y = 0; 
		
	

	if (position.y > previousPosition.y && state.movement.at(1) == MovementState::JUMP)
		state.movement.at(1) = MovementState::FALL;

	if (!to_delete)
	{
		if (position.x < 0)   // TODO: Add right map limit blocking
			position.x = 0;

		collider->SetPos(position.x, position.y);
		collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);

		if (collider->rect.h != lastPosCollider.h)
		{
			float yOffset = collider->rect.h - lastPosCollider.h;
			position.y -= yOffset;
			collider->SetPos(position.x, position.y);
			collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h, position.x, position.y);
		}


	}


	// - - - - - - - - - - - - - - - - - - warn other modules about the pos if needed
	WarnOtherModules(); 

		
	return true;
}


void j1EntityPlayer::WarnOtherModules()
{

	// - - - - - - - - - - - - - - - - >> Render 

	if ((lastSpeed.x > 0 || onDynamicplatform) && -(int)position.x < App->render->camera.x - App->render->camera.w + (int)App->render->screenDivisions.lateralValue)
	{
		App->render->DoCameraScroll(cameraScrollType::GRADUAL, direction::RIGHT, this); 
	}
	else if ((lastSpeed.x < 0|| onDynamicplatform) && -(int)position.x > App->render->camera.x - (int)App->render->screenDivisions.lateralValue && (int)previousPosition.x > (int)App->render->screenDivisions.lateralValue)
	{
		App->render->DoCameraScroll(cameraScrollType::GRADUAL, direction::LEFT, this);
	}
}



void j1EntityPlayer::OnCollision(Collider* c1, Collider* c2)
{
	bool lastOnplatform = onPlatform;

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:

		if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
		{
			if (dynamic_cast<j1EntityPlatformDynamic*>(c2->callback)->movementType == AXIS_Movement::HORIZONTAL)
			{
				if (c2->callback->pointingDir == POINTING_DIR::RIGHT)
					position.x += c2->callback->speed * App->GetDt();
				else if (c2->callback->pointingDir == POINTING_DIR::LEFT)
					position.x -= c2->callback->speed * App->GetDt();
			}



			collider->SetPos(position.x, position.y);
		}





		if (state.movement.at(1) != MovementState::JUMP)
		{
			if (!onPlatform)
			{
				if (c2->callback)    // platforms
				{
					if (collider->rect.y + collider->rect.h > c2->rect.y)
					{
						if (lastSpeed.y > 0)
						{
							if (lastAirPos.y + lastPosCollider.h - jumpComfortCornerThreshold > c2->rect.y)    //prevent the case when falling, and colliding laterally  
							{
								if ((lastGroundPos.x + lastPosCollider.w < c2->rect.x && lastSpeed.x > 0)
									|| (lastGroundPos.x > c2->rect.x + c2->rect.w) && lastSpeed.x < 0)    // when last ground was to the left and you go right or it was in the right and you go left 
								{
									float offset = 0.f;
									if (lastSpeed.x > 0)
									{
										offset = collider->rect.x + collider->rect.w - c2->rect.x;
										position.x -= offset;
									}
									else if (lastSpeed.x < 0)
									{

										offset = c2->rect.x + c2->rect.w - collider->rect.x;
										position.x += offset;
									}

									onPlatform = false;
									if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
										onDynamicplatform = false;
									ResetGravity();

									state.movement.at(1) = MovementState::FALL;

									collider->SetPos(position.x, position.y);
								}

							}
							else
							{
								if (collider->rect.y + collider->rect.h > c2->rect.y)
								{

									if (lastSpeed.y > 0)
									{

										float offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back player if it goes off a bit
										position.y -= offset;   // TODO:  this is causing a bug when already in vertical platform 

										onPlatform = true;
										if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
											onDynamicplatform = true;
										ResetGravity();

										state.movement.at(0) = MovementState::IDLE;
										state.movement.at(1) = MovementState::NOT_ACTIVE;   // jump or fall not active

										collider->SetPos(position.x, position.y);
									}
								}
							}

						}


					}

				}
				else  // just for the base floor 
				{
					if (collider->rect.y + collider->rect.h > c2->rect.y)
					{
						if (lastSpeed.y > 0)
						{
							float offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back player if it goes off a bit
							position.y -= offset;

							onPlatform = true;
							if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
								onDynamicplatform = true;
							ResetGravity();

							state.movement.at(0) = MovementState::IDLE;
							state.movement.at(1) = MovementState::NOT_ACTIVE;   // jump or fall not active

							collider->SetPos(position.x, position.y);
						}
					}
				}


			}

		}
		else if (state.movement.at(1) == MovementState::JUMP)
		{
			// Y - bottom to top                                   
			if (collider->rect.y < c2->rect.y + c2->rect.h)
			{
				if (lastSpeed.y < 0)
				{
					if ((lastGroundPos.x + lastPosCollider.w < c2->rect.x && lastSpeed.x > 0)
						|| (lastGroundPos.x > c2->rect.x + c2->rect.w) && lastSpeed.x < 0)    // when last ground was to the left and you go right or it was in the right and you go left 
					{
						float offset = 0.f;
						if (lastSpeed.x > 0)
						{
							offset = collider->rect.x + collider->rect.w - c2->rect.x;
							position.x -= offset;
						}
						else if (lastSpeed.x < 0)
						{

							offset = c2->rect.x + c2->rect.w - collider->rect.x;
							position.x += offset;
						}

						onPlatform = false;
						if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
							onDynamicplatform = false;
						ResetGravity();

						state.movement.at(1) = MovementState::FALL;

						collider->SetPos(position.x, position.y);

					}
					else
					{

						float offset = c2->rect.y + c2->rect.h - collider->rect.y;   // to put back player if it goes off a bit
						position.y += offset;

						onPlatform = false;
						if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
							onDynamicplatform = false;

						ResetGravity();

						state.movement.at(1) = MovementState::FALL;

						collider->SetPos(position.x, position.y);

					}
				}



			}


		}

		break;


	case COLLIDER_TYPE::COLLIDER_WALL:

		float offset;

		if (pointingDir == RIGHT && lastSpeed.x > 0)
		{
			if (collider->rect.x + collider->rect.w > c2->rect.x)
			{

				offset = collider->rect.x + collider->rect.w - c2->rect.x;
				position.x -= offset;
				collider->SetPos(position.x, position.y);
			}
			
				
		}
	    else if (pointingDir == LEFT && lastSpeed.x < 0)
		{
			if (collider->rect.x < c2->rect.x + c2->rect.w)
			{
				offset = c2->rect.x + c2->rect.w - collider->rect.x;
				position.x += offset;
				collider->SetPos(position.x, position.y);
			}
		}

	break;


	}


	if (!lastOnplatform && onPlatform)
	{
		lastPlatform = dynamic_cast<j1EntityPlatform*>(c2->callback);

		App->audio->PlayFx("fall");
	}
		
}

void j1EntityPlayer::OnCollisionExit(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		if (c2->hasCallback && c2->callback->type != ENTITY_TYPE::ENTITY_DYNAMIC)
		{
			if (state.movement.at(1) != MovementState::JUMP && state.movement.at(0) != MovementState::IDLE)
			{
				if (onPlatform)
				{

					if (lastSpeed.y > 0);
					{
						onPlatform = false;
						ResetGravity();

						state.movement.at(1) = MovementState::FALL;

						if (lastPlatform)
							lastPlatform = nullptr;
					


					}

				}

			}
		}
		else if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
		{
			if (onPlatform)
			{
	
					onPlatform = false;
					if (onDynamicplatform)
						onDynamicplatform = false;

					ResetGravity();

					state.movement.at(1) = MovementState::FALL;
		

					if (lastPlatform)
						lastPlatform = nullptr;
			}

		}
		
		break;

	}



	previousPosition = position;

	if (onPlatform)
		lastGroundPos = position;
	else
		lastAirPos = position;
	
}

POINTING_DIR j1EntityPlayer::GetDirection()
{

	if (lastSpeed.x < 0)
		return pointingDir = POINTING_DIR::LEFT;
	else if (lastSpeed.x > 0)
		return pointingDir = POINTING_DIR::RIGHT;
	
	return pointingDir;     // no speed results in no dir change
}



/*
bool j1EntityPlayer::InputCombat()
{
	

	// reset combat state
	combat_state = combatState::IDLE;
	

	return true;
}
*/



