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
	blitOrder = 2U; 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data

	isEnemy = true; 
	position = previousPosition = fPoint(posX, posY);
	originTilePos = App->map->WorldToMap(posX, posY); 
	pointingDir = LEFT;
	state.combat = eCombatState::IDLE;
	state.movement.at(0) = eMovementState::IDLE;
	state.movement.at(1) = eMovementState::NOT_ACTIVE;
	state.path = ePathState::FOLLOW_PLAYER; 

}	

j1Enemy::~j1Enemy()
{
}



bool j1Enemy::Move(float dt)
{

	BROFILER_CATEGORY("Enemy Move", Profiler::Color::AliceBlue);

	if (colliderActive == false)
		return false; 

	if (App->entityFactory->IsPlayerAlive() == false) 
	{
		return false;
	}
		
	if (state.combat == eCombatState::DEAD)
		return false;

	bool ret = false; 

	SetPreviousFrameData(); 

	if (state.combat != eCombatState::DYING && state.combat != eCombatState::DEAD)
	{
		if (App->render->isRectOnCamera(collider->rect))
		{
			SetPath(dt, ret);

			if (pathType != enemyPathType::FLYING)
				VerticalMovement(dt);
		}

	
	}
	else
		DieLogic(dt);

	SetCollider();    

	
	


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

	BROFILER_CATEGORY("Enemy SetPath", Profiler::Color::Gainsboro);

	bool doIt = false;

	if (state.path == ePathState::AWAIT || state.path == ePathState::TEMPORAL_DEVIATION)
	{


		if (hasMaxDistanceFromOrigin == true)
		{
			if (App->entityFactory->isInDistanceModule(originTilePos, GetTilePosition(), maxDistFromOrigin) == true)
			{
				playerInsideZone = true;
			}
			else if (App->entityFactory->isInDistanceModule(originTilePos, App->entityFactory->player->GetTilePosition(), maxDistFromOrigin) == true)
			{
				playerInsideZone = true;
			}
		}
		else
			playerInsideZone = true;

		if (playerInsideZone)
		{
			if (FollowPath(dt))
				success = true;
			else
				success = false;
		}
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

			if (hasMaxDistanceFromOrigin == true)
			{
				if (App->entityFactory->isInDistanceModule(originTilePos, GetTilePosition(), maxDistFromOrigin) == true)
				{
					playerInsideZone = true;
				}
				else if (App->entityFactory->isInDistanceModule(originTilePos, App->entityFactory->player->GetTilePosition(), maxDistFromOrigin) == true)
				{
					playerInsideZone = true;
				}
			}
			else
				playerInsideZone = true;

			if (playerInsideZone)
			{

				if (FollowPath(dt))
					success = true; 
			}
			else
				success = false;

		}
		else
			success = false;

	}
	
	playerInsideZone = success; 

	if (success == false)
		state.path = ePathState::AWAIT; 

	
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
		
	 


		if (onPlatform && deathPosGround.IsZero() == true)
		{
			deathPosGround.y = position.y + collider->rect.h;         // make so when enemy dies and anim changes, he visually stays inmovile in platform 

		}
		else if (deathPosGround.IsZero() == false)
		{

			if (!onDynamicplatform)
				position.y = deathPosGround.y - lastPosCollider.h;

		
		}

		collider->SetPos(position.x, position.y);
		collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);


	CheckDeathFinished();
}

void j1Enemy::SetCollider()
{
	if (!to_delete)
	{
		if (position.x < 0)   
			position.x = 0;
		else if (position.x > App->map->mapLimitXWorldPos)
			position.x = App->map->mapLimitXWorldPos;

		collider->SetPos(position.x, position.y);
		if(adaptativeColliderMovement)
			collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h);

		if (collider->rect.h != lastPosCollider.h)
		{
			float yOffset = collider->rect.h - lastPosCollider.h;
			position.y -= yOffset;
			collider->SetPos(position.x, position.y);

			if(adaptativeColliderMovement)
				collider->AdaptCollider(currentAnimation->GetCurrentFrame().w, currentAnimation->GetCurrentFrame().h, position.x, position.y);
		}


	}
}


void j1Enemy::VerticalMovement(float dt)
{

	// - - - - - - - - - - - - - - - - - - vertical movement

	if (doJump)
	{
		App->audio->PlayFx("dash");                   
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
		if (pathType == enemyPathType::A_TO_B)
		{
			if(targetTilePos.x < tilePos.x)
				CallPathCreation(tilePos + iPoint(1, 3), targetTilePos + iPoint(1, 3), ret);
			else
				CallPathCreation(tilePos + iPoint(0, 3), targetTilePos + iPoint(0, 3), ret);

			
		}
			
		else
			CallPathCreation(tilePos + iPoint(0, 1), targetTilePos + iPoint(0, 1), ret);
	

		if (ret)
		{
			fPoint WorldTargetPos = fPoint(0, 0);
			if (pathType == enemyPathType::A_TO_B)
			{
				if (pointingDir == RIGHT)
					WorldTargetPos.x = (float)(App->map->MapToWorld(pathToFollow.at(1).x, 0).x);
				else
					WorldTargetPos.x = (float)(App->map->MapToWorld(pathToFollow.at(1).x, 0).x);
			}
			else
				WorldTargetPos.x = (float)(App->map->MapToWorld(pathToFollow.at(1).x, 0).x);
			
		
			WorldTargetPos.y = (float)(App->map->MapToWorld(0, pathToFollow.at(1).y).y);
			iPoint tileTargetPos = App->map->WorldToMap(WorldTargetPos.x, WorldTargetPos.y); 

			iPoint WorldPosTileAdjusted = App->map->MapToWorld(tilePos.x, tilePos.y);


			if(pathType != enemyPathType::A_TO_B)
				direction = fPoint(WorldTargetPos.x - (float)WorldPosTileAdjusted.x, WorldTargetPos.y - (float)WorldPosTileAdjusted.y);
			else if (pathType == enemyPathType::A_TO_B)
			{
				if (App->pathfinding->IsWalkable(tileTargetPos))
				{
					direction = fPoint(WorldTargetPos.x - (float)WorldPosTileAdjusted.x, WorldTargetPos.y - (float)WorldPosTileAdjusted.y);
				}
			}
			else
				return ret = false; 

		}

	}
	else
		direction = fPoint((int)specificDir.x, (int)specificDir.y);


	if(state.path != ePathState::AWAIT)
		SolveMove(direction, dt);
	
	

		



	

	return ret; 

}



bool j1Enemy::CheckPathState(iPoint tilePos, iPoint& targetTilePos, bool& success)
{
	// Play detection FX acording to player position


	if (isPlayerOnMyZone() == false)
		playerNearby = false; 
	else
	{
		if (playerNearby == false)
		{
			App->audio->PlayFx(name + "Nearby");
			playerNearby = true; 
		}
	}

	// - - - - - - - - - - - - - - - - - - - Check Path State

	success = true;

	if (isPlayerOnMeleeRange() == true)    
	{
		if (pathType == enemyPathType::A_TO_B)
		{

			if (isPlayerOnSamePlatform() == true)    // if player is on melee range, but on another platform, ignore him
			{
				if (specificDir.IsZero() == false)
					specificDir = iPoint(0, 0);

				state.path = ePathState::AWAIT;
				success = false;

			}
		}
		else
		{
			if (specificDir.IsZero() == false)
				specificDir = iPoint(0, 0);

			state.path = ePathState::AWAIT;
			success = false;
		}
		
	}

	if (state.path == ePathState::AWAIT)
	{
		state.movement.at(0) = eMovementState::IDLE; 
		lastSpeed = fPoint(0, 0);

		if (isPlayerOnMeleeRange() == false && isPlayerOnMyZone() == true)           // when exiting melee range, set to follow player
			state.path = ePathState::FOLLOW_PLAYER;
		else
		    success = false;
	}

	if (state.path == ePathState::FOLLOW_PLAYER)
	{
		if (isPlayerOnMyZone() == false)           
			state.path = ePathState::AWAIT;

		targetPos.value = App->entityFactory->player->position;
		targetTilePos = App->map->WorldToMap(targetPos.value.x, targetPos.value.y);

	}
	else if (state.path == ePathState::TEMPORAL_DEVIATION)
	{

		if (isPlayerOnMyZone() == false)
		{
		 	state.path = ePathState::AWAIT;
		    success = false; 
		}
			

		targetTilePos.x = targetPos.value.x;
		targetTilePos.y = targetPos.value.y;

		bool c1 = HasArrivedToTarget(tilePos, targetTilePos);
		bool c2 = isPlayerOnMeleeRange();
		 
		if (c1 == true || c2 == true)                   
		{                                                     
			if(c1 == true)   
				state.path = ePathState::FOLLOW_PLAYER;   // when exiting melee deviation, set to follow player...
			if (c2 == true)
			{
				if (pathType == enemyPathType::A_TO_B)
				{

					if (isPlayerOnSamePlatform() == true)
					{
						state.path = ePathState::AWAIT;
						success = false;
					}
						 
					
				}
				else
				{
					state.path = ePathState::AWAIT;      
					success = false;
				}
				
			}
				
			if (specificDir.IsZero() == false)
				specificDir = iPoint(0, 0);
	
			ResolvePathDeviation();
		}

		 

	}


	return success; 
}

bool j1Enemy::HasArrivedToTarget(iPoint tilePos, iPoint targetTilePos)
{

	if (targetPos.type == TargetPos::targetPosType::XY)
	{
		if (tilePos.DistanceManhattan(targetTilePos) <= myMeleeRange)
			return true; 
	}
	else if (targetPos.type == TargetPos::targetPosType::X)
	{
		if (pathType == enemyPathType::A_TO_B)
			if (pointingDir == LEFT)
				targetTilePos.x -= 1; 

		if (abs(targetTilePos.x - tilePos.x) <= myMeleeRange)
			return true;
	}
	else if (targetPos.type == TargetPos::targetPosType::Y)
	{
		if (abs(targetTilePos.y - tilePos.y) <= myMeleeRange)
			return true;
	}

	return false; 
}

void j1Enemy::CallPathCreation(iPoint pos, iPoint target, bool& success)
{
	success = false; 

	bool walkableAccounts = false; 

	if (pathType == enemyPathType::A_TO_B)
	{
		if (onPlatform == false)
			return; 
		walkableAccounts = true;
	}
		

	

	if (pos.DistanceManhattan(target) > 1)       // The enemy doesnt collapse with the player
	{

		if (pos.DistanceManhattan(target) > 200)
		{
			success = false; 
			return; 
		}

		if (App->pathfinding->CreatePathAStar(pos, target, walkableAccounts) != 0)
		{
			pathToFollow = *App->pathfinding->GetLastPath();

			if (pathToFollow.size() > 0)
				pathToFollow.erase(pathToFollow.begin());		// Enemy doesnt go to the center of his initial tile

			success = (pathToFollow.size() > 1);
		}
		else
		{
			LOG("Could not create path correctly, enemy tried to go from (%i,%i) to (%i,%i)", pos.x, pos.y, target.x, target.y);
			success = false;
			return; 
		}


	}


}




void j1Enemy::SolveMove(fPoint Direction, float dt)
{
	if (state.movement.at(0) == eMovementState::IDLE)   // first line prevention: useful when enemies stop to attack
		return; 

	fPoint initialDirection = Direction; 

	if(Direction.IsZero() == false)
		Direction.Normalize();


	if (pathType != enemyPathType::FLYING)
	{
		if (Direction.x > 0)
			Direction.x = 1;
		else if (Direction.x < 0)
			Direction.x = -1; 
	}
	
	if (isnan(Direction.x) == true || isnan(Direction.y) == true)
	{
		LOG("Something went wrong with enemy!! Outside map");
		return; 
	}
		


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

		GetDirection(); 
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

			if (state.combat == eCombatState::SHOOT)
				currentAnimation = &attack;
			else
				if (state.path == ePathState::AWAIT)
					currentAnimation = &idle; 

			
	
}



bool j1Enemy::isPlayerOnMeleeRange() const 
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;

	iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);
	iPoint playerTilePos = App->map->WorldToMap((int)App->entityFactory->player->position.x, (int)App->entityFactory->player->position.y) + iPoint(0, 1);
	return tilePos.DistanceManhattan(playerTilePos) <= myMeleeRange; 
	
}

bool j1Enemy::isPlayerOnMyZone() const
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;

	if (hasMaxDistanceFromOrigin == false)
		return true; 

	iPoint playerTilePos = App->entityFactory->player->GetTilePosition();
	return  App->entityFactory->isInDistanceModule(playerTilePos, originTilePos, maxDistFromOrigin);

}

bool j1Enemy::isPlayerOnSamePlatform() const
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;

	return onPlatform == true && App->entityFactory->player->onPlatform == true && lastPlatform == App->entityFactory->player->lastPlatform; 
}




bool j1Enemy::DoMeleeAttack()
{

	if (App->entityFactory->IsPlayerAlive() == false)
		return false;

	if (state.combat == eCombatState::DYING)
		return false;


	if (currentAttackType == ATTACK_TYPE::LONG_RANGE)
		return false; 

	if (isPlayerOnMeleeRange() == false)     // prevent bugged attack anim when player leaves enemy behind 
	{
		currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE;     // retreat 
		state.combat = eCombatState::IDLE;

		if (lastSpeed.IsZero())
			currentAnimation = &idle;   // go back to default anim depending on enemy speed
		else
			currentAnimation = &run;

		return false; 
	}



	uint now = SDL_GetTicks();

	if (pathType == enemyPathType::FLYING || (pathType != enemyPathType::FLYING && onPlatform))  // land enemeies check they are on ground to attack
	{
		if (state.combat == eCombatState::SHOOT)
		{
			if (currentAnimation != &attack)
				currentAnimation = &attack;

			if (currentAnimation == &attack && currentAnimation->Finished() == true)  // hit is over 
			{
				/*if (isPlayerOnMeleeRange() == true)   // do damage --> with cacodemon does not work, he is 2 tiles away
				{*/
					float ShotsPerSec = 1.f / (cadenceValues.melee / 1000.f);
					App->entityFactory->DoDamagetoEntity(App->entityFactory->player, damageValues.melee, ShotsPerSec);

					if(dataAnimFx.firstAttackFxIsMelee == true)
						App->audio->PlayFx(name + "Attack");
					

				//}

					// retreat 

				currentAttackType = ATTACK_TYPE::NO_ATTACK_TYPE;     
				state.combat = eCombatState::IDLE;

				// go back to default anim depending on enemy speed

				if (lastSpeed.IsZero())
				{
					state.movement.at(0) = eMovementState::IDLE; 
					currentAnimation = &idle;   
				}
					
				else
					currentAnimation = &run;

			}
			else
				return true; 

		}
		else if (state.combat == eCombatState::IDLE)
		{
			if (now >= currentAttackData.lastTimeMeleeAttack + cadenceValues.melee)
			{
				currentAttackType = ATTACK_TYPE::MELEE;
				state.combat = eCombatState::SHOOT;
				currentAnimation = &attack;
				currentAnimation->Reset();
				currentAttackData.lastTimeMeleeAttack = now;

				return true; 
			}

		}
	}

	return false; 

}


bool j1Enemy::DoLongRangeAttack()    
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;

	if (state.combat == eCombatState::DYING)
		return false; 


	if (currentAttackType == ATTACK_TYPE::MELEE)   // do not attack if melee is already active 
		return false;
	  
	if (isPlayerOnMeleeRange() == true)   // when player is on melle range, switch instantly to melee attack
	{
		currentAttackType = ATTACK_TYPE::MELEE; 
		return false;
	}

	if (App->render->isRectOnCamera(collider->rect) == false)   // do not shoot when camera leaves me behind etc 
		return false; 


	uint now = SDL_GetTicks();

	if (state.combat == eCombatState::SHOOT)
	{
		bool c1 = (currentAnimation == &attack) ? true : false; 
		bool c2 = (currentAnimation->Finished()) ? true : false;

		// take into account a possible delay 
		if (now >= currentAttackData.lastTimeLongRangeAttack + longRangeShootData.msWaitFromAnimStartToShot && currentAttackData.lastShooted == false)
		{
			currentAttackData.lastShooted = true;
			SpawnShotParticle();
		}
			

		if ((c1 == true && c2 == true) || c1 == false)                                  // hit is over 
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
			if (now >= currentAttackData.lastTimeLongRangeAttack + cadenceValues.longRange)
			{
				currentAttackData.lastShooted = false;
				currentAttackType = ATTACK_TYPE::LONG_RANGE;
				state.combat = eCombatState::SHOOT;
				currentAnimation = &attack;
				currentAnimation->Reset();
				currentAttackData.lastTimeLongRangeAttack = now;

				 

				return true;
			}
		}
		

	}
	


	return false; 
}

void j1Enemy::SpawnShotParticle()
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return; 

	fPoint dir = GetShotDir();
	fPoint speed = GetShotSpeed(dir);
	fPoint OriginShotPos = fPoint(0, 0);

	OriginShotPos.y = position.y + longRangeShootData.relativeOffsetPos.y;
	SDL_RendererFlip shotFlip = SDL_FLIP_NONE; 

	if(speed.x < 0)
		shotFlip = SDL_FLIP_HORIZONTAL;

	if (GetDirection() == LEFT)
	{
		OriginShotPos.x = position.x + longRangeShootData.relativeOffsetPos.x;
	}

	else if (GetDirection() == RIGHT)
	{
		OriginShotPos.x = position.x + collider->rect.w - longRangeShootData.relativeOffsetPos.x - App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().w;
	}
 
	

	Particle* shot = App->particles->AddParticleRet(name + "Shot", (int)OriginShotPos.x, (int)OriginShotPos.y, this, true, COLLIDER_ENEMY_SHOT, speed, 0U,
		shotFlip);

	std::string fxname = ""; 
	if (dataAnimFx.hasSecondAttackFx == true)
		fxname = name + "Attack2"; 
	else
		fxname = name + "Attack";

	shot->fx = fxname;  // it will be played only when the particle is spawned (synchro with delay) 
}

fPoint j1Enemy::GetShotDir()
{

	//capture player and enemy positions : take into account shot offset and shot hieght
	float yOffset = longRangeShootData.relativeOffsetPos.y + App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().h / 2;
	float xOffset = longRangeShootData.relativeOffsetPos.x + App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().w / 2;

	if (pointingDir == RIGHT)
		xOffset = collider->rect.w - longRangeShootData.relativeOffsetPos.x - App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().w / 2;

	fPoint p1 = { position.x + xOffset, position.y + yOffset };
	fPoint p2 = App->entityFactory->GetRectCentralPoint(&App->entityFactory->player->collider->rect);

	fPoint Dir = p2 - p1;

	if(Dir.IsZero() == false)
		Dir.Normalize();


	if (Dir.x > 0)
		lastShotDir = POINTING_DIR::RIGHT; 
	else if (Dir.x < 0)
		lastShotDir = POINTING_DIR::LEFT;


	return Dir; 


}


void j1Enemy::OnCollision(Collider* c1, Collider* c2)
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return; 

	if (c1->type == COLLIDER_ENEMY_SHOT)
	{
		if (c2->type == COLLIDER_PLAYER || c2->type == COLLIDER_WALL || c2->type == COLLIDER_FLOOR)
		{
			if (c2->type == COLLIDER_PLAYER)
			{

				float ShotsPerSec = 1.f / (cadenceValues.longRange / 1000.f);
				App->entityFactory->DoDamagetoEntity(App->entityFactory->player, damageValues.longRange, ShotsPerSec, c1->speed);
			}

			c1->owner->to_delete = true;   // delete the shot particle AND  // create the explosion particle
			App->particles->AddParticle(name + "ShotExplosion", c1->owner->position.x, c1->owner->position.y, this, false, COLLIDER_NONE, { 0,0 }, 0U,
				flip);

		}
	}
	
	bool lastOnplatform = onPlatform;

	switch (c2->type)
	{


	case COLLIDER_TYPE::COLLIDER_FLOOR:


		/*for (const auto& col : collider->onCollisionWithMe)   // if im on the floor already, and my head is touching a platform above 
			if (col->type == COLLIDER_FLOOR)
				if (col != c2)
					return; */

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


		if (c1->type == COLLIDER_ENEMY_SHOT)
		{
			c1->owner->to_delete = true;

			App->particles->AddParticle(name + "ShotExplosion", c1->owner->position.x, c1->owner->position.y, this, false, COLLIDER_NONE, { 0,0 }, 0U,
				flip);
			return;
		}


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


		if (!lastOnplatform && onPlatform && state.combat != eCombatState::DYING)   
		{

				lastPlatform = dynamic_cast<j1EntityPlatform*>(c2->callback);
				App->audio->PlayFx("fall");
			

		}

//	}
		
}

void j1Enemy::OnCollisionExit(Collider* c1, Collider* c2)
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return;


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




POINTING_DIR j1Enemy::GetDirection()
{

	bool justMovement = false; 

	if (state.combat != eCombatState::SHOOT)
	{
		justMovement = true;
	}
	else
	{
		if (currentAttackType == ATTACK_TYPE::LONG_RANGE)
		{

			if (isAiming() == true)
			{
				justMovement = true;
			}
			else
			{
				bool c1 = (currentAnimation == &attack) ? true : false;
				bool c2 = (currentAnimation->Finished() == false) ? true : false;

				if ((c1 == true && c2 == true))                                  // hit is over 
				{
					if (lastShotDir == POINTING_DIR::LEFT)
						pointingDir = POINTING_DIR::LEFT;
					else if (lastShotDir == POINTING_DIR::RIGHT)
						pointingDir = POINTING_DIR::RIGHT;

				}
			}
			
			
		
		}
		else if(currentAttackType == ATTACK_TYPE::MELEE)
			justMovement = true; 

	}


	if (justMovement)
	{
		if (lastSpeed.x < 0)
			 pointingDir = POINTING_DIR::LEFT;
		else if (lastSpeed.x > 0)
			 pointingDir = POINTING_DIR::RIGHT;

		  
		if (state.movement.at(0) == eMovementState::IDLE || isAiming() == true)
			if (App->entityFactory->player->position.x + App->entityFactory->player->collider->rect.w / 2 >= position.x + collider->rect.w / 2)
				pointingDir = POINTING_DIR::RIGHT;
			else
				pointingDir = POINTING_DIR::LEFT; 
	}



	if (pointingDir == POINTING_DIR::LEFT)
		state.movement.at(0) = eMovementState::INPUT_LEFT; 
	else if (pointingDir == POINTING_DIR::RIGHT)
		state.movement.at(0) = eMovementState::INPUT_RIGHT;
	
	return pointingDir; 
}


bool j1Enemy::Go_A_to_B()
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;


	if (onPlatform == true)
	{
			if (state.path != ePathState::TEMPORAL_DEVIATION || resetAtoB == true)
			{
				speed = platFormSpeed;

				if (pointingDir == LEFT)
					targetPos.value = fPoint(App->map->WorldToMap(lastPlatform->collider->rect.x, 0));

				else if (pointingDir == RIGHT)
				{
					targetPos.value = fPoint(App->map->WorldToMap(lastPlatform->collider->rect.x + lastPlatform->collider->rect.w, 0));
					targetPos.value.x -= 2;
				}


				targetPos.value.y = GetTilePosition().y;
				targetPos.type = TargetPos::targetPosType::X;
				state.path = ePathState::TEMPORAL_DEVIATION;

				resetAtoB = false; 
			}

	}


	return true; 
}




bool j1Enemy::isWallBetweenPlayerAndMe(bool shoot)
{
	//capture player and enemy positions : take into account shot offset and shot hieght
	if (App->entityFactory->IsPlayerAlive() == false)
		return false;


	fPoint p1 = fPoint(0, 0); 
	if (shoot == true)
	{
		float yOffset = longRangeShootData.relativeOffsetPos.y + App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().h / 2;
		float xOffset = longRangeShootData.relativeOffsetPos.x + App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().w / 2;

		if (pointingDir == RIGHT)
			xOffset = collider->rect.w - longRangeShootData.relativeOffsetPos.x - App->particles->GetParticleAt(name + "Shot").anim.GetCurrentFrame().w / 2;

		p1 = { position.x + xOffset, position.y + yOffset };
	}
	else
		p1 = App->entityFactory->GetRectCentralPoint(&collider->rect);
	

	SDL_Rect target = { App->entityFactory->player->position.x, App->entityFactory->player->position.y, App->entityFactory->player->collider->rect.w,
	App->entityFactory->player->collider->rect.h }; 
	fPoint p2 = App->entityFactory->GetRectCentralPoint(&target);


	// make a line and store it for debugging purposes 
	lastRaycastInfo.lastRaycast = { (int)p1.x, (int)p1.y, (int)p2.x, (int)p2.y };


	// check if any wall on screen has got an intersection with the line 
	for (const auto& wallCol : App->collision->colliders)
		if (wallCol && (wallCol->type == COLLIDER_WALL || wallCol->type == COLLIDER_FLOOR))
			if (App->render->IsOnCamera(wallCol->rect.x, wallCol->rect.y, wallCol->rect.w, wallCol->rect.h) == true)
				if (App->entityFactory->hasIntersectionRectAndLine(&wallCol->rect, lastRaycastInfo.lastRaycast) == true)
				{
					lastRaycastInfo.Color = { 255, 0, 0, 255 };
					return true;
				}


	lastRaycastInfo.Color = { 0, 255, 0, 255 };
	return false;

}



void j1Enemy::SetDyingState(bool brutal)
{
	if (state.combat == eCombatState::DYING || state.combat == eCombatState::DEAD)
		return;


	// if i was blocking the player movement, unblock it: 

	if (App->entityFactory->player->isParalized() == true)
	{
		for (const auto& col : collider->onCollisionWithMe)
			if (col->type == COLLIDER_PLAYER)
				App->entityFactory->player->UnParalize();
	}

	state.combat = eCombatState::DYING;
	blitOrder = 1U;  // to be rendered under weapons etc

	App->audio->StopSpecificFx(name + "Injured");   // so that death is audible 

	if (!brutal || dataAnimFx.hasSecondDeathAnim == false)   // check this out (for the ones that only have one death anim) 
	{
		currentAnimation = &death1;
		App->audio->PlayFx(this->name + "Death");   
	}

	else
	{
		currentAnimation = &death2;

		if (dataAnimFx.hasSecondDeathFx)
			App->audio->PlayFx(this->name + "Death2");   
		else
			App->audio->PlayFx(this->name + "Death");  
	}

}


bool j1Enemy::Load(pugi::xml_node &node)
{
	j1Entity::Load(node);

	auto attackNode = node.child("attack_data");
	std::string currentAttackValue = attackNode.child("current_attack").attribute("value").as_string();

	//  - - - - - - - - - - - - - - - - ATTACK DATA

	if (currentAttackValue == "melee")
		currentAttackType = MELEE; 
	else if (currentAttackValue == "long_range")
		currentAttackType = LONG_RANGE;
	else if (currentAttackValue == "no_attack_type")
		currentAttackType = NO_ATTACK_TYPE;

	currentAttackData.lastTimeMeleeAttack = attackNode.child("last_melee").attribute("value").as_uint();
	currentAttackData.lastShooted = attackNode.child("last_shooted").attribute("value").as_uint();


	if (damageValues.longRange != 0U)
	{
		currentAttackData.lastTimeLongRangeAttack = attackNode.child("last_long_range").attribute("value").as_uint();
		std::string shotDir = attackNode.child("last_long_range_direction").attribute("value").as_string();

		if (shotDir == "right")
			lastShotDir = RIGHT;
		else if (shotDir == "left")
			lastShotDir = LEFT; 
	}


	//  - - - - - - - - - - - - - - - - ANIMATION DATA
	std::string animNodeValue = node.child("animation_data").attribute("anim").as_string();

	if (animNodeValue == "idle")
		currentAnimation = &idle;
	else if (animNodeValue == "run")
		currentAnimation = &run; 
	else if (animNodeValue == "attack")
		currentAnimation = &attack;
	else if (animNodeValue == "death1")
		currentAnimation = &death1;
	else if (animNodeValue == "death2")
		currentAnimation = &death2;

	std::string animNodeDir = node.child("animation_data").attribute("last_direction").as_string();

	if (animNodeDir == "right")
		lastPointingDir = RIGHT; 
	else if (animNodeDir == "left")
		lastPointingDir = LEFT;

	float animFrame = node.child("animation_data").attribute("current_animation_frame").as_float(); 
	currentAnimation->SetCurrentFrame(animFrame);

	//  - - - - - - - - - - - - - - - - EXTRA POSITION DATA
	auto posInfo = node.child("extra_position_data");
	auto lastGround = posInfo.child("last_ground_pos");
	lastGroundPos.x = lastGround.attribute("x").as_float();
	lastGroundPos.y = lastGround.attribute("y").as_float();
	auto lastAir = posInfo.child("last_air_pos");
	lastAirPos.x = lastAir.attribute("x").as_float();
	lastAirPos.y = lastAir.attribute("y").as_float();

	auto targetPosNode = posInfo.child("target_pos_data");
	auto targetPosValue = targetPosNode.child("target_pos_value");
	targetPos.value.x = targetPosValue.attribute("x").as_float();
	targetPos.value.y = targetPosValue.attribute("y").as_float();

 
	std::string targetPosTypeValue = targetPosNode.child("target_pos_type").attribute("value").as_string();

	if (targetPosTypeValue == "X")
		targetPos.type = TargetPos::targetPosType::X; 
	else if (targetPosTypeValue == "Y")
		targetPos.type = TargetPos::targetPosType::Y;
	else if (targetPosTypeValue == "XY")
		targetPos.type = TargetPos::targetPosType::XY;


	auto deathGround = posInfo.child("death_ground_pos");
	deathPosGround.x = deathGround.attribute("x").as_float(); 
	deathPosGround.y = deathGround.attribute("y").as_float();

	auto specificDirection = posInfo.child("specific_dir");
	specificDir.x =	specificDirection.attribute("x").as_int();
	specificDir.y = specificDirection.attribute("y").as_int();

	//  - - - - - - - - - - - - - - - - STATE DATA
	auto stateNode = node.child("state_data");
	std::string combatStateNodeValue = stateNode.child("combat").attribute("value").as_string();

	if (combatStateNodeValue == "dead")
		state.combat = eCombatState::DEAD; 
	else if (combatStateNodeValue == "dying")
			state.combat = eCombatState::DYING;
	else if (combatStateNodeValue == "idle")
		state.combat = eCombatState::IDLE;
	else if (combatStateNodeValue == "shoot")
		state.combat = eCombatState::SHOOT;

	std::string movementStateNodeValue0 = stateNode.child("movement").attribute("value_0").as_string();

	if (movementStateNodeValue0 == "idle")
		state.movement.at(0) = eMovementState::IDLE; 
	else if (movementStateNodeValue0 == "input_right")
		state.movement.at(0) = eMovementState::INPUT_RIGHT;
	else if (movementStateNodeValue0 == "input_left")
		state.movement.at(0) = eMovementState::INPUT_LEFT;
	else if (movementStateNodeValue0 == "run")
		state.movement.at(0) = eMovementState::RUN;

	std::string movementStateNodeValue1 = stateNode.child("movement").attribute("value_1").as_string();

	if (movementStateNodeValue1 == "fall")
		state.movement.at(1) = eMovementState::FALL;
	else if (movementStateNodeValue1 == "jump")
		state.movement.at(1) = eMovementState::JUMP;
	else if (movementStateNodeValue1 == "not_active")
		state.movement.at(1) = eMovementState::NOT_ACTIVE;

	std::string pathStateNodeValue = stateNode.child("path").attribute("value").as_string();

	if (pathStateNodeValue == "await")
		state.path = ePathState::AWAIT; 
	else if (pathStateNodeValue == "temporal_deviation")
		state.path = ePathState::TEMPORAL_DEVIATION;
	else if (pathStateNodeValue == "follow player")
		state.path = ePathState::FOLLOW_PLAYER;

	//  - - - - - - - - - - - - - - - -  COMBAT DATA
	life = node.child("combat_data").child("life").attribute("value").as_float();


	//  - - - - - - - - - - - - - - - - OTHER DATA
	auto other = node.child("other_data");
	auto lastCol = other.child("last_pos_collider");
	lastPosCollider.x = lastCol.attribute("x").as_int();
	lastPosCollider.y = lastCol.attribute("y").as_int();
	lastPosCollider.w =	lastCol.attribute("w").as_int();
	lastPosCollider.h = lastCol.attribute("h").as_int();

	playerNearby = other.child("player_nearby").attribute("value").as_bool();
	playerInsideZone = 	other.child("player_inside_zone").attribute("value").as_bool();
	onPlatform = other.child("on_platform").attribute("value").as_bool();
	onDynamicplatform = other.child("on_dynamic_platform").attribute("value").as_bool();


	if (this->type == ENTITY_TYPE::ENEMY_IMP)
	{
		doJump = other.child("do_jump").attribute("value").as_bool();
		jumpInfo.currenJumpPower = other.child("current_jump_power").attribute("value").as_float();
	}

	if (this->pathType == enemyPathType::A_TO_B)
		resetAtoB = other.child("reset_a_to_b").attribute("value").as_bool();

	stopLogic = other.child("stop_logic").attribute("value").as_bool();

	auto raycastData = other.child("raycast_data");
	auto ray = raycastData.child("ray");
	lastRaycastInfo.lastRaycast.at(0) = ray.attribute("x1").as_int();
	lastRaycastInfo.lastRaycast.at(1) = ray.attribute("y1").as_int();
	lastRaycastInfo.lastRaycast.at(2) = ray.attribute("x2").as_int();
	lastRaycastInfo.lastRaycast.at(3) = ray.attribute("y2").as_int();

	auto color = raycastData.child("color");
	lastRaycastInfo.Color.r = color.attribute("R").as_uint();
	lastRaycastInfo.Color.g = color.attribute("G").as_uint();
	lastRaycastInfo.Color.b = color.attribute("B").as_uint();
	lastRaycastInfo.Color.a = color.attribute("A").as_uint();


	//  - - - - - - - - - - - - - - - - RELATED ENTIIES DATA

	// get last saved platform entity ID and retrieve it from the factory: 
	if (onPlatform == true)
	{
		uint lastPlatformID = node.child("related_entities_data").child("last_platform_ID").attribute("value").as_uint();
		lastPlatform = (j1EntityPlatform*)App->entityFactory->GetEntityFromID(lastPlatformID); 
	
	}

	return true;

}

bool j1Enemy::Save(pugi::xml_node &node) const
{
	j1Entity::Save(node);

	auto attackNode = node.append_child("attack_data"); 
	auto currentAttack = attackNode.append_child("current_attack");

	//  - - - - - - - - - - - - - - - - ATTACK DATA
	switch (currentAttackType)
	{
	case MELEE:
		currentAttack.append_attribute("value") = "melee";
		break;
	case LONG_RANGE:
		currentAttack.append_attribute("value") = "long_range";
		break;
	case NO_ATTACK_TYPE:
		currentAttack.append_attribute("value") = "no_attack_type";
		break;
	default:
		break;
	}
	 
	attackNode.append_child("last_melee").append_attribute("value") = currentAttackData.lastTimeMeleeAttack;
	attackNode.append_child("last_shooted").append_attribute("value") = currentAttackData.lastShooted;

	if (damageValues.longRange != 0U)
	{
		attackNode.append_child("last_long_range").append_attribute("value") = currentAttackData.lastTimeLongRangeAttack;
		auto shotDir = attackNode.append_child("last_long_range_direction"); 
		switch (lastShotDir)
		{
		case RIGHT:
			shotDir.append_attribute("value") = "right";
			break;
		case LEFT:
			shotDir.append_attribute("value") = "left";
			break;
		default:
			break;
		}
	}
		

	//  - - - - - - - - - - - - - - - - ANIMATION DATA
	auto animNode = node.append_child("animation_data");
	 
	if(currentAnimation == &idle)
		animNode.append_attribute("anim") = "idle";
	else if (currentAnimation == &run)
		animNode.append_attribute("anim") = "run";
	else if (currentAnimation == &attack)
		animNode.append_attribute("anim") = "attack";
	else if (currentAnimation == &death1)
		animNode.append_attribute("anim") = "death1";
	else if (currentAnimation == &death2)
		animNode.append_attribute("anim") = "death2";

	 
	switch (lastPointingDir)
	{
	case RIGHT:
		animNode.append_attribute("last_direction") = "right";
		break;
	case LEFT:
		animNode.append_attribute("last_direction") = "left";
		break;
	default:
		break;
	}

	animNode.append_attribute("current_animation_frame") = currentAnimation->GetCurrentFloatFrame();


	//  - - - - - - - - - - - - - - - - EXTRA POSITION DATA
	auto posInfo = node.append_child("extra_position_data");
	auto lastGround = posInfo.append_child("last_ground_pos"); 
	lastGround.append_attribute("x") = lastGroundPos.x;
	lastGround.append_attribute("y") = lastGroundPos.y;
	auto lastAir = posInfo.append_child("last_air_pos");
	lastAir.append_attribute("x") = lastAirPos.x;
	lastAir.append_attribute("y") = lastAirPos.y;
   
	auto targetPosNode = posInfo.append_child("target_pos_data");
	auto targetPosValue = targetPosNode.append_child("target_pos_value");
	targetPosValue.append_attribute("x") = targetPos.value.x;
	targetPosValue.append_attribute("y") = targetPos.value.y;
	auto targetPosType = targetPosNode.append_child("target_pos_type");
	switch (targetPos.type) 
	{
	case TargetPos::targetPosType::X:
		targetPosType.append_attribute("value") = "X";
		break; 
	case TargetPos::targetPosType::Y:
		targetPosType.append_attribute("value") = "Y";
		break;
	case TargetPos::targetPosType::XY:
		targetPosType.append_attribute("value") = "XY";
		break;
	default:
		break;
	}

	auto deathGround = posInfo.append_child("death_ground_pos");
	deathGround.append_attribute("x") = deathPosGround.x;
	deathGround.append_attribute("y") = deathPosGround.y;
 
	auto specificDirection = posInfo.append_child("specific_dir");
	specificDirection.append_attribute("x") = specificDir.x;
	specificDirection.append_attribute("y") = specificDir.y;

	//  - - - - - - - - - - - - - - - - STATE DATA
	auto stateNode = node.append_child("state_data");
	auto combatStateNode = stateNode.append_child("combat");
	switch (state.combat)
	{
	case eCombatState::DEAD:
		combatStateNode.append_attribute("value") = "dead";
		break; 
	case eCombatState::DYING:
		combatStateNode.append_attribute("value") = "dying";
		break;
	case eCombatState::IDLE:
		combatStateNode.append_attribute("value") = "idle";
		break;
	case eCombatState::SHOOT:
		combatStateNode.append_attribute("value") = "shoot";
		break;
	default:
		break;
	}
	auto movementStateNode = stateNode.append_child("movement");

	switch (state.movement.at(0))
	{
	case eMovementState::IDLE: 
		movementStateNode.append_attribute("value_0") = "idle";
		break; 
	case eMovementState::INPUT_LEFT: 
		movementStateNode.append_attribute("value_0") = "input_left";
		break; 
	case eMovementState::INPUT_RIGHT:
		movementStateNode.append_attribute("value_0") = "input_right";
		break;
	case eMovementState::RUN:
		movementStateNode.append_attribute("value_0") = "run";
		break;
	default:
		break;
	}

	switch (state.movement.at(1))
	{
	case eMovementState::FALL:
		movementStateNode.append_attribute("value_1") = "fall";
		break;
	case eMovementState::JUMP:
		movementStateNode.append_attribute("value_1") = "jump";
		break;
	case eMovementState::NOT_ACTIVE:
		movementStateNode.append_attribute("value_1") = "not_active";
		break;
	default:
		break;
	}

	auto pathStateNode = stateNode.append_child("path");


	switch (state.path)
	{
	case ePathState::AWAIT:
		pathStateNode.append_attribute("value") = "await";
		break; 
	case ePathState::TEMPORAL_DEVIATION:
		pathStateNode.append_attribute("value") = "temporal_deviation";
		break;
	case ePathState::FOLLOW_PLAYER:
		pathStateNode.append_attribute("value") = "follow_player";
		break;
	default:
		break;
	}

	//  - - - - - - - - - - - - - - - -  COMBAT DATA
	node.append_child("combat_data").append_child("life").append_attribute("value") = life;

	//  - - - - - - - - - - - - - - - - RELATED ENTIIES DATA
	auto relatedEntities = node.append_child("related_entities_data");

	if (lastPlatform != nullptr)
		relatedEntities.append_child("last_platform_ID").append_attribute("value") = lastPlatform->ID; 
 
	//  - - - - - - - - - - - - - - - - OTHER DATA
	auto other = node.append_child("other_data");
	auto lastCol = other.append_child("last_pos_collider");
	lastCol.append_attribute("x") = lastPosCollider.x;
	lastCol.append_attribute("y") = lastPosCollider.y;
	lastCol.append_attribute("w") = lastPosCollider.w;
	lastCol.append_attribute("h") = lastPosCollider.h;

	other.append_child("player_nearby").append_attribute("value") = playerNearby;
	other.append_child("player_inside_zone").append_attribute("value") = playerInsideZone;
	other.append_child("on_platform").append_attribute("value") = onPlatform;
	other.append_child("on_dynamic_platform").append_attribute("value") = onDynamicplatform;

	if(this->type == ENTITY_TYPE::ENEMY_IMP)
	{
		other.append_child("do_jump").append_attribute("value") = doJump;
		other.append_child("current_jump_power").append_attribute("value") = jumpInfo.currenJumpPower;
	}

	if(this->pathType == enemyPathType::A_TO_B)
		other.append_child("reset_a_to_b").append_attribute("value") = resetAtoB;
	other.append_child("stop_logic").append_attribute("value") = stopLogic;
 
	auto raycastData = other.append_child("raycast_data");
	auto ray = raycastData.append_child("ray"); 
	ray.append_attribute("x1") = lastRaycastInfo.lastRaycast.at(0);
	ray.append_attribute("y1") = lastRaycastInfo.lastRaycast.at(1);
	ray.append_attribute("x2") = lastRaycastInfo.lastRaycast.at(2);
	ray.append_attribute("y2") = lastRaycastInfo.lastRaycast.at(3);
	auto color = raycastData.append_child("color");
	color.append_attribute("R") = lastRaycastInfo.Color.r; 
	color.append_attribute("G") = lastRaycastInfo.Color.g;
	color.append_attribute("B") = lastRaycastInfo.Color.b;
	color.append_attribute("A") = lastRaycastInfo.Color.a;

	return true;

};



