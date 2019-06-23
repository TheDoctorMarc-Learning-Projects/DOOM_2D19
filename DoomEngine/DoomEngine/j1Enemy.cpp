#include "j1Enemy.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "j1Scene.h"
#include <math.h>


j1Enemy::j1Enemy(int posX, int posY) : j1Entity(ENEMY_STATIC, posX, posY, "enemy")
{

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	position = previousPosition = fPoint(posX, posY);
	pointingDir = LEFT;
	state.combat = eCombatState::IDLE;
	state.movement.at(0) = eMovementState::IDLE;
	state.movement.at(1) = eMovementState::NOT_ACTIVE;

	

}	

j1Enemy::~j1Enemy()
{
}

bool j1Enemy::Start()
{

	return true;
}

bool j1Enemy::PreUpdate()
{

	return true;
}

bool j1Enemy::Update(float dt)
{

	return true;
}

bool j1Enemy::PostUpdate()
{

	return true;
}

bool j1Enemy::CleanUp()
{

	collider->to_delete = true;

	if (entityTex != nullptr)
		App->tex->UnLoad(entityTex);


	return true;
}

bool j1Enemy::Load(pugi::xml_node &)
{
	return true;
}

bool j1Enemy::Save(pugi::xml_node &) const
{
	return true;
}



bool j1Enemy::Move(float dt)
{

	previousPosition = position;

	if (onPlatform)
		lastGroundPos = position;
	else
		lastAirPos = position;


	if (!onPlatform)
		onDynamicplatform = false;

	lastPosCollider = collider->rect;

	FollowPlayer(); 

	if (lastSpeed.x != 0)
	{
		currentAnimation = &run;

		int multiplier = 5; 
		if (lastSpeed.x > 0)
			multiplier = !multiplier; 

		if (state.movement.at(1) != eMovementState::NOT_ACTIVE)
		{
			if (state.movement.at(1) == eMovementState::JUMP)
				lastSpeed.x = (multiplier * speed) * jumpInfo.speedXIncrementJump * dt;
			else if (state.movement.at(1) == eMovementState::FALL)
				lastSpeed.x = (multiplier * speed) * jumpInfo.speedXIncrementFall * dt;

		}
		else
			lastSpeed.x = (multiplier * speed) * dt;

		state.movement.at(0) = (lastSpeed.x < 0) ? eMovementState::INPUT_LEFT : state.movement.at(0);
		state.movement.at(0) = (lastSpeed.x > 0) ? eMovementState::INPUT_RIGHT : state.movement.at(0);

		position.x += lastSpeed.x;

	}
		
	else
	{
		if (state.movement.at(0) != eMovementState::IDLE)
		{
			state.movement.at(0) = eMovementState::IDLE;
			//currentAnimation = &idle;

		}

		currentAnimation = &idle;
		lastSpeed.x = 0;
	}


	// - - - - - - - - - - - - - - - - - - vertical movement

	if (!onPlatform)
	{
		if (state.movement.at(1) == eMovementState::FALL)
		{
			lastSpeed.y = GravityCalc(gravityFactor, mass) * dt;
			position.y += lastSpeed.y;
		}

		else if (state.movement.at(1) == eMovementState::JUMP)
		{
			if (lastSpeed.x == 0)
				lastSpeed.y = (-(jumpInfo.currenJumpPower *= jumpInfo.jumpIncrementFactor * jumpInfo.verticalIncrementFactor)) + GravityCalc(gravityFactor, mass) * dt;

			else
				lastSpeed.y = (-(jumpInfo.currenJumpPower *= jumpInfo.jumpIncrementFactor)) + GravityCalc(gravityFactor, mass) * dt;

			position.y += lastSpeed.y;

			if (lastSpeed.y > 0)
				state.movement.at(1) = eMovementState::FALL;

		}

	}
	else
		lastSpeed.y = 0;



	if (position.y > previousPosition.y && state.movement.at(1) == eMovementState::JUMP)
		state.movement.at(1) = eMovementState::FALL;

	if (position.x < 0)   // TODO: Add right map limit blocking
		position.x = 0;

	if (!to_delete)
	{
		collider->SetPos(position.x, position.y);
		collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);
	}


	// - - - - - - - - - - - - - - - - - - warn other modules about the pos if needed
	WarnOtherModules();


	return true;
}


void j1Enemy::WarnOtherModules()
{


}

void j1Enemy::FollowPlayer()
{

}


void j1Enemy::OnCollision(Collider* c1, Collider* c2)
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





		if (state.movement.at(1) != eMovementState::JUMP)
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

								state.movement.at(1) = eMovementState::FALL;

								collider->SetPos(position.x, position.y);


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

										state.movement.at(0) = eMovementState::IDLE;
										state.movement.at(1) = eMovementState::NOT_ACTIVE;   // jump or fall not active

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

							state.movement.at(0) = eMovementState::IDLE;
							state.movement.at(1) = eMovementState::NOT_ACTIVE;   // jump or fall not active

							collider->SetPos(position.x, position.y);
						}
					}
				}


			}

		}
		else if (state.movement.at(1) == eMovementState::JUMP)
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

						state.movement.at(1) = eMovementState::FALL;

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

						state.movement.at(1) = eMovementState::FALL;

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
		App->audio->PlayFx("fall2");
}

void j1Enemy::OnCollisionExit(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		if (c2->hasCallback && c2->callback->type != ENTITY_TYPE::ENTITY_DYNAMIC)
		{
			if (state.movement.at(1) != eMovementState::JUMP && state.movement.at(0) != eMovementState::IDLE)
			{
				if (onPlatform)
				{

					if (lastSpeed.y > 0);
					{
						onPlatform = false;
						ResetGravity();

						state.movement.at(1) = eMovementState::FALL;
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

				state.movement.at(1) = eMovementState::FALL;

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
