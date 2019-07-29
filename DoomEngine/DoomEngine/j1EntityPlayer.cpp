#include "j1EntityPlayer.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1EntityFactory.h" // needed for some functions
#include "j1Enemy.h"

#include <math.h>


j1EntityPlayer::j1EntityPlayer(int posX, int posY, std::string name) : j1Entity(PLAYER, posX , posY, "player")
{

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name); 
	useRenderFlip = true; 
	blitOrder = 4U; 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = PLAYER; 
	this->name = name;
	maxLife = life = 500;        // TODO: show life in percentatge UI, not this value 
	maxArmor = 300; 
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
	idle.PushBack({18, 421, size.x, size.y}); 

	run.PushBack({ 8, 33, size.x + 8, size.y });
	run.PushBack({ 4, 130, size.x + 3, size.y + 2 });
	run.PushBack({ 7, 227, size.x + 4, size.y + 1});
	run.PushBack({ 7, 324, size.x, size.y + 1 });
	run.loop = true; 
	run.speed = 3.f; 

	aimUp.PushBack({ 70, 33, size.x + 6, size.y + 2 });

	aimDown.PushBack({ 70, 422, size.x, size.y - 1 });

	death1.PushBack({ 48, 688, size.x + 6, size.y - 3 });
	death1.PushBack({ 132, 688, size.x + 6, size.y - 3 });
	death1.PushBack({ 211, 688, size.x + 2, size.y - 3 });
	death1.PushBack({ 291, 688, size.x + 7, size.y - 16 });
	death1.PushBack({ 376, 688, size.x + 17, size.y - 39 });
	death1.PushBack({ 471, 688, size.x + 17, size.y - 39 });
	death1.PushBack({ 566, 688, size.x + 17, size.y - 39 });
	death1.speed = 3.f;
	death1.loop = false; 

	death2.PushBack({ 46, 771, size.x + 2, size.y + 3 });
	death2.PushBack({ 126, 771, size.x + 4, size.y + 6 });
	death2.PushBack({ 214, 771, size.x + 14, size.y + 4 });
	death2.PushBack({ 306, 771, size.x + 19, size.y - 1 });
	death2.PushBack({ 402, 771, size.x + 21, size.y - 6 });
	death2.PushBack({ 502, 771, size.x + 23, size.y - 13 });
	death2.PushBack({ 603, 771, size.x + 23, size.y - 20 });
	death2.PushBack({ 704, 771, size.x + 23, size.y - 30 });
	death2.PushBack({ 805, 771, size.x + 23, size.y - 31 });
	death2.speed = 3.f;
	death2.loop = false;

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
	
	if (App->input->GetKey(SDL_SCANCODE_G) == KEY_DOWN)
		godMode = !godMode; 

 

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
	BROFILER_CATEGORY("Player Move", Profiler::Color::Aqua);
	SetPreviousFrameData();

    if (state.combat != combatState::DYING && state.combat != combatState::DEAD)
	{
		
		HorizonatlMovement(dt);
		VerticalMovement(dt);
		WeaponLogic();
		SetCollider();
		// - - - - - - - - - - - - - - - - - - warn other modules about the pos if needed
		WarnOtherModules();
	}
	else
		DieLogic(dt); 

	

	return true;
}

void j1EntityPlayer::DieLogic(float dt)
{
	if (!onPlatform)
	{
		VerticalMovement(dt);
	}




	if (onPlatform && deathPosGround.IsZero() == true)
	{
		deathPosGround.y = position.y + collider->rect.h;         // make so when enemy dies and anim changes, he visually stays inmovile in platform 
	//	deathColllider = collider->rect;

	}
    if (deathPosGround.IsZero() == false)
	{

		if (!onDynamicplatform)
			position.y = deathPosGround.y - lastPosCollider.h;


	}

	collider->SetPos(position.x, position.y);
	collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);


	if (!myWeapons.empty())
	{
		currentWeapon->playerKO = true;
		currentWeapon->FallToTheFloor(dt);
	}



	CheckDeathFinished();
}


bool j1EntityPlayer::CleanUp()
{
	j1Entity::CleanUp(); 

	myWeapons.clear();   // the content is destroyed in their clean up

	return true; 
}

void j1EntityPlayer::SetPreviousFrameData()
{
	previousPosition = position;

	if (onPlatform)
		lastGroundPos = position;
	else
		lastAirPos = position;

	if (!onPlatform)
		onDynamicplatform = false;

	lastPosCollider = collider->rect;
}


void j1EntityPlayer::HorizonatlMovement(float dt)
{
	if (state.combat != combatState::AIM)
	{
		Sint16 xAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_LEFTX);


		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
			xAxis = SDL_JOYSTICK_AXIS_MAX; 
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
			xAxis = -SDL_JOYSTICK_AXIS_MAX;

		// - - - - - - - - - - - - - - - - - - horizontal movement
		if (xAxis > 0 || xAxis < 0)
		{


			currentAnimation = &run;

			if (state.movement.at(1) != MovementState::NOT_ACTIVE)
			{
				if (state.movement.at(1) == MovementState::JUMP)
					lastSpeed.x = (xAxis * speed) * jumpInfo.speedXIncrementJump* dt;
				else if (state.movement.at(1) == MovementState::FALL)
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
	}


}

void j1EntityPlayer::VerticalMovement(float dt)
{
	// - - - - - - - - - - - - - - - - - - vertical movement


	if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
		//if (yAxis <= 0)
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
			if (lastSpeed.x == 0)
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
}

void j1EntityPlayer::SetCollider()
{
	if (position.x < 0)    
		position.x = 0;

	else if (position.x > App->map->mapLimitXWorldPos)  // TODO: uncomment this
		position.x = App->map->mapLimitXWorldPos;

	collider->SetPos(position.x, position.y);
	collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);

	if (onPlatform)
	{
		if (collider->rect.h != lastPosCollider.h)                         // do some corrections not to go through floor 
		{
			float yOffset = collider->rect.h - lastPosCollider.h;
	
			if (position.y + collider->rect.h - yOffset <= lastPlatform->position.y)
			{
				position.y -= yOffset;

				
			}
			else
			{
				position.y = lastPlatform->position.y - collider->rect.h; 
			}

			collider->SetPos(position.x, position.y);
			collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h, position.x, position.y);

		}

		if (collider->rect.w != lastPosCollider.w)
		{
			float xOffset = collider->rect.w - lastPosCollider.w;

			if (state.combat != combatState::AIM)
			{
				if (pointingDir == POINTING_DIR::RIGHT)  // do corrections when changing anim to aim, so player stays visually in the same pos 
				{
					position.x -= xOffset;
				}
			}
			else
			{
				if (pointingDir == POINTING_DIR::LEFT)
				{
					position.x -= xOffset;
				}
				
			
			}
		
				
				

			collider->SetPos(position.x, position.y);
			collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h, position.x, position.y);
		}
	}

}

void j1EntityPlayer::WeaponLogic()
{
	
	if (!myWeapons.empty())
	{

		// capture input 
		if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		{
			ChangeWeapon(SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
		}

		if (App->input->GetControllerButton(SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_J) == KEY_DOWN)
		{
			ChangeWeapon(SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
		}

		//AimWeapon(); 

		j1KeyState shootButtonState = App->input->GetControllerAxisPulsation(SDL_CONTROLLER_AXIS_TRIGGERRIGHT);

		if (App->input->GetKey(SDL_SCANCODE_K) != KEY_IDLE)
			shootButtonState = App->input->GetKey(SDL_SCANCODE_K);
	
		if(shootButtonState != KEY_IDLE)
			ShootWeapon(shootButtonState);
		
		

	}

}


void j1EntityPlayer::ChangeWeapon(SDL_GameControllerButton button)
{
	if (myWeapons.size() >= 2)  // at least 2 weapons for the functionality to occur 
	{
		int i = 0;
		for (auto weapon = myWeapons.begin(); weapon != myWeapons.end(); ++weapon)
		{
			if ((*weapon)->weaponData.weaponState == WEAPON_STATE::ACTIVE)
			{
				// 1) first deEquip the current weapon
				(*weapon)->weaponData.weaponState = WEAPON_STATE::INACTIVE;
				(*weapon)->drawActive = false;
				(*weapon)->collider->to_delete = true; 

				if ((*weapon)->firing)
					(*weapon)->firing = false; 

				// chainsaw hotspot collider needs to be deleted too: 
				if ((*weapon)->GetWeaponType() == WEAPON_TYPE::CHAINSAW)
					(*weapon)->AddHotSpotToChainsaw(false);

				(*weapon)->StopAllFxs(); 


				currentWeapon = nullptr; 

				int desiredIndex = 0; 
				// 2) then equip next or last weapon in list
				if (button == SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) 
				{
					if (i < myWeapons.size() - 1 || i == 0)    // prevent trying to get a weapon not in the list
					{
						desiredIndex = i + 1; 
					}
					else  // jump to the first weapon in the list
					{
						desiredIndex = 0; 
					}
					
				}
				else if (button == SDL_CONTROLLER_BUTTON_LEFTSHOULDER)
				{
					if (i > 0)    // prevent trying to get a weapon not in the lsit 
					{
						desiredIndex = i - 1; 
					}
					else // jump to the last weapon in the list
					{
						desiredIndex = myWeapons.size() - 1; 
				    }
			    }


				if (myWeapons.at(desiredIndex)->weaponData.weaponState == WEAPON_STATE::INACTIVE)
				{
					myWeapons.at(desiredIndex)->weaponData.weaponState = WEAPON_STATE::ACTIVE;
					myWeapons.at(desiredIndex)->drawActive = true;


					// add a collider again needed to draw
					myWeapons.at(desiredIndex)->collider = App->collision->AddCollider({ myWeapons.at(desiredIndex)->section.x, myWeapons.at(desiredIndex)->section.y,
						(int)((float)myWeapons.at(desiredIndex)->section.w * myWeapons.at(desiredIndex)->spriteScale),
						(int)((float)myWeapons.at(desiredIndex)->section.h * myWeapons.at(desiredIndex)->spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, myWeapons.at(desiredIndex));

					// chainsaw needs a hotspot collider to do damage: 
					if (myWeapons.at(desiredIndex)->GetWeaponType() == WEAPON_TYPE::CHAINSAW)
						myWeapons.at(desiredIndex)->AddHotSpotToChainsaw(true);

					myWeapons.at(desiredIndex)->PlaceMeWithPlayer();
					currentWeapon = myWeapons.at(desiredIndex); 




				
				}
			

				break; 
			} 
		

			++i;
		}

		
	}
		

	


}

void j1EntityPlayer::AimWeapon()
{

	if (state.movement.at(0) == MovementState::IDLE && (state.combat == combatState::IDLE || state.combat == combatState::AIM))  // have to be idle to aim, to give some challenge, but you can run and shoot horizontally
	{
		if (currentWeapon->GetFiringType() != firingType::MELEE && currentWeapon->GetWeaponType() != WEAPON_TYPE::CHAINSAW)   // no aim with chainsaw etc
		{

			Sint16 yAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_RIGHTY);
			Sint16 xAxis = App->input->GetControllerAxis(SDL_CONTROLLER_AXIS_RIGHTX);

			if (xAxis == 0 && yAxis == 0)    // prevention on release
			{
				currentAnimation = &idle; 
				state.combat = combatState::IDLE;    
				currentWeapon->ChangeRotation((double)0);
				return; 
			}
			
			state.combat = combatState::AIM;
 
			// first consider if player has to change pointing direction, so he can aim in any direction

			if (xAxis > 0 && pointingDir == LEFT)
				pointingDir = RIGHT;
			else if (xAxis < 0 && pointingDir == RIGHT)
				pointingDir = LEFT; 

			// then alter the player animation according to axis thresholds and then alter the current weapon sprite rotation

			int partition = MAX_CONTROLLER_AXIS / 3;  // divide positive Y positive axis in 3 partitions, as well as another 3 for the negative 

			if (yAxis < -MAX_CONTROLLER_AXIS + 2 * partition)
			{
				currentAnimation = &aimUp;

				if(pointingDir == RIGHT)
					currentWeapon->ChangeRotation(-(double)comfortShootAngle);
				else if(pointingDir == LEFT)
					currentWeapon->ChangeRotation((double)comfortShootAngle);
				
			}
				
			else if (yAxis > -MAX_CONTROLLER_AXIS + 2 * partition && yAxis < MAX_CONTROLLER_AXIS - 2 * partition)
			{
				currentAnimation = &idle;
				currentWeapon->ChangeRotation(0);
			}
				
			else
			{
				currentAnimation = &aimDown;
				

				if (pointingDir == RIGHT)
					currentWeapon->ChangeRotation((double)comfortShootAngle);
				else if (pointingDir == LEFT)
					currentWeapon->ChangeRotation(-(double)comfortShootAngle);

			}
				

			

			
		}

	}

}

void j1EntityPlayer::ShootWeapon(j1KeyState state)
{

	currentWeapon->Shoot(state);

}

void j1EntityPlayer::WarnOtherModules()
{
	// - - - - - - - - - - - - - - - - >> Loot (weapon)

	if(!myWeapons.empty())
		for (auto& weapon : myWeapons)
			if(weapon->weaponData.weaponState == WEAPON_STATE::ACTIVE)
				weapon->PlaceMeWithPlayer();

	

	// - - - - - - - - - - - - - - - - >> Render 
	int speed = -15; 

	if (pointingDir == LEFT)
		speed = -speed; 

	if ((lastSpeed.x > 0 || onDynamicplatform) && -(int)position.x < App->render->camera.x - App->render->camera.w + (int)App->render->screenDivisions.lateralValue)
	{
		App->render->SetCameraScroll(cameraScrollType::GRADUAL, direction::RIGHT, speed, 650); 
	}
	else if ((lastSpeed.x < 0|| onDynamicplatform) && -(int)position.x > App->render->camera.x - (int)App->render->screenDivisions.lateralValue && (int)previousPosition.x > (int)App->render->screenDivisions.lateralValue)
	{
		App->render->SetCameraScroll(cameraScrollType::GRADUAL, direction::LEFT, speed, 650);
	}
}



void j1EntityPlayer::OnCollision(Collider* c1, Collider* c2)
{
	bool lastOnplatform = onPlatform;
	float ShotsPerSec = 0.0f;

	switch (c2->type)
	{
		
		

		break;


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


	 

		
		if (!onPlatform)
		{
			if (c2->callback)    // platforms
			{
				if (collider->rect.y + collider->rect.h >= c2->rect.y)
				{
					if (lastSpeed.y > 0)
					{
						if (lastPosCollider.y + lastPosCollider.h > c2->rect.y)    
						{
							if ((lastPosCollider.x + lastPosCollider.w <= c2->rect.x && lastSpeed.x > 0)
								|| (lastPosCollider.x >= c2->rect.x + c2->rect.w) && lastSpeed.x < 0)    // when last ground was to the left and you go right or it was in the right and you go left 
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


								if ((collider->rect.x + collider->rect.w <= c2->rect.x || position.x >= c2->rect.x + c2->rect.w) && lastSpeed.x == 0)
									return; 

								if (position.y + collider->rect.h > c2->rect.y + c2->rect.h)
									return; 
								

								float offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back player if it goes off a bit
								position.y -= offset;   // TODO:  this is causing a bug when already in vertical platform 

								onPlatform = true;
								if (c2->hasCallback && c2->callback->type == ENTITY_TYPE::ENTITY_DYNAMIC)
									onDynamicplatform = true;
								ResetGravity();

								state.movement.at(0) = MovementState::IDLE;
								state.movement.at(1) = MovementState::NOT_ACTIVE;   // jump or fall not active

								collider->SetPos(position.x, position.y);


							
								currentAnimation->Resume();
							}
						}

					}


				}

			}
			else  // just for the base floor 
			{
				if (collider->rect.y + collider->rect.h >= c2->rect.y)
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


						 
						currentAnimation->Resume();
					}
				}
			}




			// Y - bottom to top                                   
			if (collider->rect.y <= c2->rect.y + c2->rect.h)
			{
				if (lastSpeed.y < 0)
				{
					if ((lastPosCollider.x + lastPosCollider.w < c2->rect.x && lastSpeed.x > 0)
						|| (lastPosCollider.x > c2->rect.x + c2->rect.w) && lastSpeed.x < 0)    // when last ground was to the left and you go right or it was in the right and you go left 
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

						if ((collider->rect.x + collider->rect.w <= c2->rect.x || position.x >= c2->rect.x + c2->rect.w) && lastSpeed.x == 0)
							return;

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

		if (pointingDir == RIGHT /*&& lastSpeed.x > 0*/)
		{
			if (collider->rect.x + collider->rect.w > c2->rect.x && collider->rect.x < c2->rect.x)  // second condition is due to aiming and changing pointing dir and collider
			{

				offset = collider->rect.x + collider->rect.w - c2->rect.x;
				position.x -= offset;
				collider->SetPos(position.x, position.y);
			}
			
				
		}
	    else if (pointingDir == LEFT /*&& lastSpeed.x < 0*/)
		{
			if (collider->rect.x < c2->rect.x + c2->rect.w && collider->rect.x > c2->rect.x)  // second condition is due to aiming and changing pointing dir and collider
			{
				offset = c2->rect.x + c2->rect.w - collider->rect.x;
				position.x += offset;
				collider->SetPos(position.x, position.y);
			}
		}

	break;



	case COLLIDER_TYPE::COLLIDER_LOOT:

		
		if (c2->hasCallback)
		{
			if (dynamic_cast<j1EntityLoot*>(c2->callback)->GetType() == LOOT_TYPE::WEAPON)
				PickWeapon(c2);

			else if (dynamic_cast<j1EntityLoot*>(c2->callback)->GetType() == LOOT_TYPE::COIN)
				dynamic_cast<j1EntityLootCoin*>(c2->callback)->OnPickUp(); 
			else if (dynamic_cast<j1EntityLoot*>(c2->callback)->GetType() == LOOT_TYPE::HEALTH)
				dynamic_cast<j1EntityLootHealth*>(c2->callback)->OnPickUp();
			else if (dynamic_cast<j1EntityLoot*>(c2->callback)->GetType() == LOOT_TYPE::AMMO)
				dynamic_cast<j1EntityLootAmmo*>(c2->callback)->OnPickUp();
			else if (dynamic_cast<j1EntityLoot*>(c2->callback)->GetType() == LOOT_TYPE::ARMOR)
				dynamic_cast<j1EntityLootArmor*>(c2->callback)->OnPickUp();
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

		currentAnimation->Pause(); 

		if (c2->hasCallback && c2->callback->type != ENTITY_TYPE::ENTITY_DYNAMIC)
		{
			if (state.movement.at(1) != MovementState::JUMP)
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



void j1EntityPlayer::PickWeapon(Collider* c2)
{
	if (dynamic_cast<j1EntityLootWeapon*>(c2->callback)->weaponData.weaponState == WEAPON_STATE::AWAIT)   // pick weapon only if I don't have it
	{

		// first check if another weapon is active 

		for (const auto& weapon : myWeapons)
			if (weapon->weaponData.weaponState == WEAPON_STATE::ACTIVE)
			{
				weapon->weaponData.weaponState = WEAPON_STATE::INACTIVE;  // if other was active, put it to inactive
				weapon->drawActive = false;       // do not draw it --> TODO: when swappig weapon put to active both variables
				weapon->collider->to_delete = true;
				currentWeapon = nullptr;
			}

		myWeapons.push_back(dynamic_cast<j1EntityLootWeapon*>(c2->callback));
		currentWeapon = dynamic_cast<j1EntityLootWeapon*>(c2->callback);

		// chainsaw needs a hotspot collider to do damage: 
		if (dynamic_cast<j1EntityLootWeapon*>(c2->callback)->weaponData.weaponType == WEAPON_TYPE::CHAINSAW)
			dynamic_cast<j1EntityLootWeapon*>(c2->callback)->AddHotSpotToChainsaw(true);

		dynamic_cast<j1EntityLootWeapon*>(c2->callback)->weaponData.weaponState = WEAPON_STATE::ACTIVE;    // put to active 
		dynamic_cast<j1EntityLootWeapon*>(c2->callback)->PlaceMeWithPlayer();  // player the new weapon in the desired pos; 

		App->audio->PlayFx("weaponPickUp");

		dynamic_cast<j1EntityLootWeapon*>(c2->callback)->blitOrder = 3U;   // so it is under enemies, but on top when picked


		

	}
}
