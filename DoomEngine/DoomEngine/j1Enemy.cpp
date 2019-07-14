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

		if(pathType != enemyPathType::FLYING)
			VerticalMovement(dt);
	
		//SetCollider(); 
	}
	else
		DieLogic(dt);

	SetCollider();   // TODO: check this out, check commits if when it was here it was ok 

	
	


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

	if (state.path == ePathState::AWAIT || state.path == ePathState::TEMPORAL_DEVIATION)
	{
		if (FollowPath(dt))
			success = true;
		else
			success = false;
		
	}

	else if (state.path == ePathState::FOLLOW_PLAYER)
	{
		iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);
		iPoint playerTilePos = App->map->WorldToMap((int)App->entityFactory->player->position.x, (int)App->entityFactory->player->position.y) + iPoint(0, 1);

		uint Distance = (uint)(int)(float)abs(hypot(playerTilePos.x - tilePos.x, playerTilePos.y - tilePos.y));
		if (Distance <= tileDetectionRange)
		{
			if (FollowPath(dt))
				success = true;
			else
				success = false;

		}
	}
	
	
}

void j1Enemy::DieLogic(float dt)
{
   

	if (!onPlatform)
	{
		if (pathType == enemyPathType::FLYING)                         // flying enemy is able to fall only when dying 
			if (state.movement.at(1) != eMovementState::FALL)
				state.movement.at(1) = eMovementState::FALL;

		VerticalMovement(dt);
	}
		

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
		if (state.movement.at(1) == eMovementState::FALL)                // cacodemon already ignores this except when dying
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
	iPoint playerTilePos = App->map->WorldToMap((int)App->entityFactory->player->position.x, (int)App->entityFactory->player->position.y);

	if (CheckPathState(tilePos, targetTilePos, ret) == false)
		return ret; 


	fPoint direction = fPoint(0, 0);

	if (specificDir.IsZero() == true)
	{
		CallPathCreation(tilePos + iPoint(0, 1), targetTilePos + iPoint(0, 1), ret);

		if (ret)
		{
			fPoint WorldTargetPos = fPoint(0, 0);
			WorldTargetPos.x = (float)(App->map->MapToWorld(pathToFollow.at(1).x, 0).x);
			WorldTargetPos.y = (float)(App->map->MapToWorld(0, pathToFollow.at(1).y).y);
			iPoint WorldPosTileAdjusted = App->map->MapToWorld(tilePos.x, tilePos.y);

			direction = fPoint(WorldTargetPos.x - (float)WorldPosTileAdjusted.x, WorldTargetPos.y - (float)WorldPosTileAdjusted.y);


		

		}

	}
	else
		direction = fPoint((int)specificDir.x, (int)specificDir.y);

	SolveMove(direction, dt);
	

		



	

	return ret; 

}


bool j1Enemy::CheckPathState(iPoint tilePos, iPoint& targetTilePos, bool& success)
{

	if (isPlayerOnMeleeRange() == true)    
	{
		if (specificDir.IsZero() == false)
			specificDir = iPoint(0, 0);

		state.path = ePathState::AWAIT;             // on melee range = 1 tile: wait 
		return success = false;
	}

	if (state.path == ePathState::AWAIT)
	{
		if (isPlayerOnMeleeRange() == false)           // when exiting melee range, set to follow player
			state.path = ePathState::FOLLOW_PLAYER;
		else
			return success = false;

	}

	else if (state.path == ePathState::FOLLOW_PLAYER)
	{
		targetPos.value = App->entityFactory->player->position;
		targetTilePos = App->map->WorldToMap(targetPos.value.x, targetPos.value.y);

	}
	else if (state.path == ePathState::TEMPORAL_DEVIATION)
	{
		targetTilePos.x = targetPos.value.x;
		targetTilePos.y = targetPos.value.y;

		bool c1 = HasArrivedToTarget(tilePos, targetTilePos);
		bool c2 = isPlayerOnMeleeRange();
		 
		if (c1 == true || c2 == true)                   
		{                                                     
			if(c1 == true)   
				state.path = ePathState::FOLLOW_PLAYER;   // when exiting melee deviation, set to follow player...
			if(c2 == true)
				state.path = ePathState::AWAIT;      // ... or rather set or overwrite to await if on melee range

			if (specificDir.IsZero() == false)
				specificDir = iPoint(0, 0);
	
			ResolvePathDeviation();
		}

		 

	}


	return success = true; 
}

bool j1Enemy::HasArrivedToTarget(iPoint tilePos, iPoint targetTilePos)
{

	if (targetPos.type == TargetPos::targetPosType::XY)
	{
		if (tilePos.DistanceManhattan(targetTilePos) <= 1)
			return true; 
	}
	else if (targetPos.type == TargetPos::targetPosType::X)
	{
		if (abs(targetTilePos.x - tilePos.x) <= 1)
			return true;
	}
	else if (targetPos.type == TargetPos::targetPosType::Y)
	{
		if (abs(targetTilePos.y - tilePos.y) <= 1)
			return true;
	}

	return false; 
}

void j1Enemy::CallPathCreation(iPoint pos, iPoint target, bool& success)
{
	success = false; 

	bool walkableAccounts = false; 

	if (pathType == enemyPathType::A_TO_B)
		walkableAccounts = true; 

	if (pos.DistanceManhattan(target) > 1)       // The enemy doesnt collapse with the player
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

	fPoint initialDirection = Direction; 

	if(Direction.IsZero() == false)
		Direction.Normalize();
	
	if (isnan(Direction.x) == true || isnan(Direction.y) == true)
		LOG("Something went wrong with enemy!! Outside map");


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
				if (state.combat == eCombatState::IDLE)
				{
					currentAnimation = &run;
				}
				
				position.x += lastSpeed.x;
			}

		}
		else
		{
			if (lastSpeed.x != 0 || lastSpeed.y != 0)
			{
				if (state.combat == eCombatState::IDLE)
				{
					currentAnimation = &run;
				}

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
	return tilePos.DistanceManhattan(playerTilePos) <= 1; 
	
}



bool j1Enemy::DoMeleeAttack()
{
	static uint lastTimeAttack = 0;

	uint now = SDL_GetTicks();

	if (pathType == enemyPathType::FLYING || (pathType != enemyPathType::FLYING && onPlatform))  // land enemeies check they are on ground to attack
	{
		if (state.combat == eCombatState::SHOOT)
		{
			if (currentAnimation != &attack)
				currentAnimation = &attack;

			if (currentAnimation == &attack && currentAnimation->Finished() == true)  // hit is over 
			{
				if (isPlayerOnMeleeRange() == true)   // do damage 
				{
					float ShotsPerSec = 1.f / (cadenceValues.melee / 1000.f);
					App->entityFactory->DoDamagetoEntity(App->entityFactory->player, damageValues.melee, ShotsPerSec);
					App->audio->PlayFx(name + "Attack");

				}

				currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE;     // retreat 
				state.combat = eCombatState::IDLE;

				if (lastSpeed.IsZero())
					currentAnimation = &idle;   // go back to default anim depending on enemy speed
				else
					currentAnimation = &run;

			}
			else
				return true; 

		}
		else if (state.combat == eCombatState::IDLE)
		{
			if (now >= lastTimeAttack + cadenceValues.melee)
			{
				currentAttackType = ATTACK_TYPE::MELEE;
				state.combat = eCombatState::SHOOT;
				currentAnimation = &attack;
				currentAnimation->Reset();
				lastTimeAttack = now;

				return true; 
			}

		}
	}

	return false; 

}


bool j1Enemy::DoLongRangeAttack()   // TODO: Check if enemy has a special long range attack anim or audio ???? 
{
	static uint lastTimeAttack = 0;

	uint now = SDL_GetTicks();

	if (state.combat == eCombatState::SHOOT)
	{
		bool c1 = (currentAnimation == &attack) ? true : false; 
		bool c2 = (currentAnimation->Finished()) ? true : false;

		if (c1 == true && c2 == true)                                  // hit is over 
		{
			currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE;     // retreat 
			state.combat = eCombatState::IDLE;

			if (lastSpeed.IsZero())
				currentAnimation = &idle;   // go back to default anim depending on enemy speed
			else
				currentAnimation = &run;

		}
		else
			return true;

	}
	else if (state.combat == eCombatState::IDLE)
	{

		if (now < cadenceValues.longRange)   // at the start of the app, it is possible that now is less than cadence, so it won't shoot
		{
			uint capture = now; 
			now += cadenceValues.longRange - capture;
		}
		else
		{
			if (now >= lastTimeAttack + cadenceValues.longRange)
			{
				currentAttackType = ATTACK_TYPE::LONG_RANGE;
				state.combat = eCombatState::SHOOT;
				currentAnimation = &attack;
				currentAnimation->Reset();
				lastTimeAttack = now;

				SpawnShotParticle();    // create projectile the first frame

				return true;
			}
		}
		

	}
	


	return false; 
}

void j1Enemy::SpawnShotParticle()
{
	App->audio->PlayFx(name + "Attack");

	fPoint dir = GetShotDir();
	fPoint speed = fPoint(dir.x * longRangeShootData.shotSpeed, dir.y * longRangeShootData.shotSpeed);
	fPoint targetPos = fPoint(0, 0); 

	targetPos.y = position.y + longRangeShootData.relativeOffsetPos.y; 

	if (pointingDir == LEFT)
		targetPos.x = position.x + longRangeShootData.relativeOffsetPos.x;
	else if(pointingDir == RIGHT)
		targetPos.x = position.x + collider->rect.w - longRangeShootData.relativeOffsetPos.x;


	uint delay = longRangeShootData.msWaitFromAnimStartToShot;   // some delay needed to look more realistic 

	Particle* shot = App->particles->AddParticleRet(name + "Shot", (int)targetPos.x, (int)targetPos.y, this, true, COLLIDER_ENEMY_SHOT, speed, delay,
		flip);

}

fPoint j1Enemy::GetShotDir()
{
	fPoint playerPos = App->entityFactory->player->position; 
	fPoint Dir = playerPos - position; 

	if(Dir.IsZero() == false)
		Dir.Normalize();

	return Dir; 


}





void j1Enemy::OnCollision(Collider* c1, Collider* c2)
{
	bool lastOnplatform = onPlatform;

	switch (c2->type)
	{

	case COLLIDER_TYPE::COLLIDER_PLAYER:       // long range shots are detected here 

		if (c1->type == COLLIDER_ENEMY_SHOT)
		{
			c1->owner->to_delete = true;   // delete the shot particle AND  // create the explosion particle
			
			/*Particle* explosion = App->particles->AddParticleRet(name + "ShotExplosion", c1->callback->position.x, c1->callback->position.y, this, COLLIDER_PRESENTIAL, { 0,0 }, 0U,
				flip);   */

			                                                                      
			float ShotsPerSec = 1.f / (cadenceValues.longRange / 1000.f);
			App->entityFactory->DoDamagetoEntity(App->entityFactory->player, damageValues.longRange, ShotsPerSec, c1->speed);

		}

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




