#include "j1Enemy.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include "j1Scene.h"



j1Enemy::j1Enemy(int posX, int posY) : j1Entity(ENEMY_STATIC, posX, posY, "enemy")
{
	useRenderFlip = true; 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	position = previousPosition = fPoint(posX, posY);
	pointingDir = LEFT;
	state.combat = eCombatState::IDLE;
	state.movement.at(0) = eMovementState::IDLE;
	state.movement.at(1) = eMovementState::NOT_ACTIVE;
	state.path = ePathState::FOLLOW_PLAYER; 
	
	
	
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

/*bool j1Enemy::CleanUp()
{

	collider->to_delete = true;

	if (entityTex != nullptr)          // MAJOR TODO: do not delete texture, first create the entity corpse and pass it the texture 
		App->tex->UnLoad(entityTex);


	return true;
}*/

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


	BROFILER_CATEGORY("Enemy Move", Profiler::Color::AliceBlue); 

	bool ret = false; 

	SetPreviousFrameData(); 

	if (state.combat != eCombatState::DYING)
	{
		SetPath(dt, ret);
		VerticalMovement(dt);
		SetCollider(); 
	}
	else
		DieLogic(dt);

	

	
	


	return ret;
}

void j1Enemy::SetPreviousFrameData()
{
	if (!to_delete)
	{
		previousPosition = position;

		if (onPlatform)
			lastGroundPos = position;
		else
			lastAirPos = position;


		if (!onPlatform)
			onDynamicplatform = false;

		lastPosCollider = collider->rect;

		lastPointingDir = pointingDir;
	}
}

void j1Enemy::SetPath(float dt, bool& success)
{

	if (state.path == ePathState::FOLLOW_PLAYER)
	{
		iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);
		iPoint playerTilePos = App->map->WorldToMap((int)App->entityFactory->player->position.x, (int)App->entityFactory->player->position.y) + iPoint(0, 1);

		uint Distance = (uint)(int)(float)abs(hypot(playerTilePos.x - tilePos.x, playerTilePos.y - tilePos.y));
		if (Distance <= tileDetectionRange)
		{
			if (FollowPath(dt))
				success = true;

		}
	}
	else if (state.path == ePathState::TEMPORAL_DEVIATION)
	{
		success = true;
	}
	
}

void j1Enemy::DieLogic(float dt)
{
	if (!onPlatform)
		VerticalMovement(dt);

	if (onPlatform && deathPosGround.IsZero())
	{
		deathPosGround.y = position.y + collider->rect.h;         // make so when enemy dies and anim changes, he visually stays inmovile in platform 
		deathColllider = collider->rect;

	}
	else if (!deathPosGround.IsZero())
	{
		float offset = deathColllider.h - collider->rect.h;

		if (!onDynamicplatform)
			position.y = deathPosGround.y - deathColllider.h + offset;

		collider->SetPos(position.x, position.y);
		collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);
	}


	CheckDeathFinished();
}

void j1Enemy::SetCollider()
{
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
}


void j1Enemy::VerticalMovement(float dt)
{

	// - - - - - - - - - - - - - - - - - - vertical movement

	if (doJump)
	{
		App->audio->PlayFx("dash");                   // TODO: CHANGE SOUND
		state.movement.at(1) = eMovementState::JUMP;

		onPlatform = false;
		ResetGravity();


		doJump = false; 
	}

	if (!onPlatform)
	{
		if (state.movement.at(1) == eMovementState::FALL)                // cacodemon already ignores this, cannot be in these states
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


	/*if (position.y > previousPosition.y && state.movement.at(1) == eMovementState::JUMP)
		state.movement.at(1) = eMovementState::FALL;*/
}



bool j1Enemy::FollowPath(float dt)
{
	
	bool ret = false;

	BROFILER_CATEGORY("Enemy Follow Path", Profiler::Color::Azure);

	pathToFollow.clear();
	iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);
	iPoint targetTilePos = iPoint(0, 0);

	if (state.path == ePathState::FOLLOW_PLAYER)
	{
		targetPos.value = App->entityFactory->player->position;
		targetTilePos = App->map->WorldToMap(targetPos.value.x, targetPos.value.y) + iPoint(0, 1);
	}
	else if (state.path == ePathState::TEMPORAL_DEVIATION)
	{
		targetTilePos.x = targetPos.value.x;
		targetTilePos.y = targetPos.value.y;

		if (tilePos.DistanceManhattan(targetTilePos) <= 1)
		{
			state.path = ePathState::FOLLOW_PLAYER;
			ResolvePathDeviation();
		}

	}

	
	CallPathCreation(tilePos + iPoint(0, 1), targetTilePos + iPoint(0, 1), ret);

	
	if (ret)
	{
			fPoint WorldTargetPos = fPoint(0, 0);
			WorldTargetPos.x = (float)(App->map->MapToWorld(pathToFollow.at(1).x, 0).x);
			WorldTargetPos.y = (float)(App->map->MapToWorld(0, pathToFollow.at(1).y).y);

			fPoint direction = WorldTargetPos - position;

			SolveMove(direction, dt);
				
	}



	if (isPlayerOnMeleeRange())   // do it even if stunned depending on enemy, maybe some enemies cancel attacks etc 
		DoAttack(true); 


	return ret; 

}

void j1Enemy::CallPathCreation(iPoint pos, iPoint target, bool& success)
{
	bool walkableAccounts = false; 

	if (pathType == enemyPathType::A_TO_B)
		walkableAccounts = true; 

	if (pos.DistanceManhattan(target) > 0)       // The enemy doesnt collapse with the player
	{

		if (App->pathfinding->CreatePathAStar(pos, target, walkableAccounts) != 0)
		{
			pathToFollow = *App->pathfinding->GetLastPath();
			if (pathToFollow.size() > 0)
				pathToFollow.erase(pathToFollow.begin());		// Enemy doesnt go to the center of his initial tile

			success = (pathToFollow.size() > 1);
		}
		else LOG("Could not create path correctly");


	}


}




void j1Enemy::SolveMove(fPoint Direction, float dt)
{

	Direction.Normalize(); 

	// if too small, set to 0 so that enemy is idle

	if (abs(Direction.x) < 0.1f)
		Direction.x = 0.f; 
	if (abs(Direction.y) < 0.1f)
		Direction.y = 0.f; 

	// - - - - - - - - - - - - - - - - - -  X Axis

		if (state.movement.at(1) != eMovementState::NOT_ACTIVE)  // jump or fall 
		{
			if (state.movement.at(1) == eMovementState::JUMP)
				lastSpeed.x = (Direction.x * speed) * jumpInfo.speedXIncrementJump * dt;
			else if (state.movement.at(1) == eMovementState::FALL)
				lastSpeed.x = (Direction.x * speed) * jumpInfo.speedXIncrementFall * dt;

		}
		else
			lastSpeed.x = (Direction.x * speed) * dt;


	// - - - - - - - - - - - - - - - - - -  Y Axis

		if (pathType == enemyPathType::FLYING)
			lastSpeed.y = (Direction.y * speed) * dt;

	// - - - - - - - - - - - - - - - - - -  Assign Direction

		AssignDirectionWithSpeed(); 

	// - - - - - - - - - - - - - - - - - -  Assign Position and Animation


		if (pathType != enemyPathType::FLYING)
		{
			if (lastSpeed.x != 0)
			{
				currentAnimation = &run;
				position.x += lastSpeed.x;
			}

		}
		else
		{
			if (lastSpeed.x != 0 || lastSpeed.y != 0)
			{
				currentAnimation = &run;
				position.x += lastSpeed.x;
				position.y += lastSpeed.y;
			}

		}
		
	

	
}

void j1Enemy::AssignDirectionWithSpeed()
{
	state.movement.at(0) = (lastSpeed.x < 0) ? eMovementState::INPUT_LEFT : state.movement.at(0);
	state.movement.at(0) = (lastSpeed.x > 0) ? eMovementState::INPUT_RIGHT : state.movement.at(0);
	state.movement.at(0) = (lastSpeed.x == 0) ? eMovementState::IDLE : state.movement.at(0);
}

bool j1Enemy::isPlayerOnMeleeRange()
{
	iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);
	iPoint playerTilePos = App->map->WorldToMap((int)App->entityFactory->player->position.x, (int)App->entityFactory->player->position.y) + iPoint(0, 1);
	if (tilePos.DistanceManhattan(playerTilePos) <= 1)
		return true;
	return false;
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


							}
							else
							{
								if (collider->rect.y + collider->rect.h > c2->rect.y)
								{

									if (lastSpeed.y)
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


	if (!lastOnplatform && onPlatform && state.combat != eCombatState::DYING)   // TODO: same with player code 
	{
		lastPlatform = dynamic_cast<j1EntityPlatform*>(c2->callback);

		App->audio->PlayFx("fall");
	}
		
}

void j1Enemy::OnCollisionExit(Collider* c1, Collider* c2)
{

	switch (c2->type)
	{
	case COLLIDER_TYPE::COLLIDER_FLOOR:
		if ((c2->hasCallback && c2->callback->type != ENTITY_TYPE::ENTITY_DYNAMIC) || !c2->hasCallback)
		{
			if ((state.movement.at(1) != eMovementState::JUMP && state.movement.at(0) != eMovementState::IDLE || state.combat == eCombatState::DYING) )
			{
				if (onPlatform)
				{

					if (lastSpeed.y > 0);
					{
						onPlatform = false;
						ResetGravity();

						state.movement.at(1) = eMovementState::FALL;


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

				state.movement.at(1) = eMovementState::FALL;


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




