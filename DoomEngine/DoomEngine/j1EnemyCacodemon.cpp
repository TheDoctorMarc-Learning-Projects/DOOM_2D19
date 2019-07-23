#include "j1EnemyCacodemon.h" 
#include "j1EntityFactory.h"

j1EnemyCacodemon::j1EnemyCacodemon(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	adaptativeColliderMovement = false; 
	type = ENEMY_CACODEMON;
	this->name = name;
	maxLife = 480;
	life = (float)maxLife;
	size.create(62, 67);
	speed = 50.75f;
	mass = 1.8f;
	gravityFactor = DEFAULT_GRAV / mass; 
	tileDetectionRange = 8;
	cadenceValues.melee = 3000;
	cadenceValues.longRange = 3000; 
	damageValues.melee = 120;
	damageValues.longRange = 45; 
	longRangeShootData.msWaitFromAnimStartToShot = 800;
	longRangeShootData.relativeOffsetPos.create(20, size.y / 2); // assuming spritescale is 1.0f
	longRangeShootData.shotSpeed = 200;
	pathType = enemyPathType::FLYING;
	dataAnimFx.hasSecondDeathAnim = false;
	dataAnimFx.hasSecondDeathFx = false;
	dataAnimFx.firstAttackFxIsMelee = false; 
	hasMaxDistanceFromOrigin = true; 
	maxDistFromOrigin = 8U; 
	myMeleeRange = 2U; 

	state.movement.at(1) = eMovementState::NOT_ACTIVE;   // NEVER to jump or fall 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 34, 26, size.x, size.y });

	run.PushBack({ 34, 26, size.x, size.y });

	
	death1.PushBack({ 44, 645, size.x + 1, size.y});
	death1.PushBack({ 151, 645, size.x + 1, size.y });
	death1.PushBack({ 258, 645, size.x + 1, size.y });
	death1.PushBack({ 365, 645, size.x + 5, size.y + 10});
	death1.PushBack({ 476, 645, size.x + 7, size.y - 1});
	death1.PushBack({ 589, 645, size.x + 13, size.y - 18});
	death1.speed = 1.7f;
	death1.loop = false;


	attack.PushBack({ 34, 126, size.x, size.y });
	attack.PushBack({ 33, 226, size.x, size.y + 2});
	attack.PushBack({ 34, 328, size.x, size.y + 8});
	attack.PushBack({ 35, 436, size.x, size.y + 1});
	attack.PushBack({ 35, 537, size.x, size.y + 1});
	attack.speed = 3.f;
	attack.loop = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);
	
	shieldAreaCollider = App->collision->AddCollider({ (int)position.x - shieldExtraSideSize / 2, 
		(int)position.y - shieldExtraSideSize / 2, 
		(int)((float)size.x * spriteScale) + shieldExtraSideSize,
		(int)((float)size.y * spriteScale) + shieldExtraSideSize }, COLLIDER_TYPE::COLLIDER_WALL_DETECTION, this);
}

j1EnemyCacodemon::~j1EnemyCacodemon()
{
}


bool j1EnemyCacodemon::Move(float dt)
{
	if (j1Enemy::Move(dt))
	{

		shieldAreaCollider->SetPos(position.x - shieldExtraSideSize / 2, position.y - shieldExtraSideSize / 2); 

		if (keepMovingAfterPlatform)
			KeepMovingTendency(); 
	}

	if (state.combat != eCombatState::DYING && state.combat != eCombatState::DEAD)
	{
		DoMeleeAttack();
		DoLongRangeAttack();
	}
	/*else
		return false; */
	
	


	return true;
}


void j1EnemyCacodemon::KeepMovingTendency()
{
	// if pos has advanced some units, stop moving tendency
	bool tendencyOver = false; 

	if (specificDir.x == 0)
	{
		if (abs(offPlatformPos.y - position.y) >= extraPlatformTendencyOffset)
			tendencyOver = true; 
	}
	else if (specificDir.y == 0)
	{
		if (abs(offPlatformPos.x - position.x) >= extraPlatformTendencyOffset)
			tendencyOver = true;
	}

	if (tendencyOver == true)
	{
		offPlatformPos = fPoint(0, 0); 
		specificDir = iPoint(0, 0);
		state.path = ePathState::FOLLOW_PLAYER;
		keepMovingAfterPlatform = false;
	}
	   
}


bool j1EnemyCacodemon::CleanUp()    // called when exiting, or when switching levels etc 
{
	j1Entity::CleanUp(); 

	// clean the area collider: 

	if(shieldActive == true)
		shieldAreaCollider->to_delete = true;



	return true; 
}

bool j1EnemyCacodemon::DoDie()
{
	 
	j1Enemy::DoDie(); 
	// the other colldier is deleted in enemy checkdeath
	// clean the area collider: 
	shieldAreaCollider->to_delete = true;
	shieldActive = false;

	return true; 
}


void j1EnemyCacodemon::OnCollision(Collider* c1, Collider* c2)
{


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


	if (c1->type == COLLIDER_ENEMY)
	{
		if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL)
		{
			if (state.combat == eCombatState::DYING)
			{
				onPlatform = true;      // finally put general enemy on platform to true, for die logic purposes
			}
		}

	}



	if (c1->type == COLLIDER_WALL_DETECTION)
	{
		if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL)
		{

			if(c2->hasCallback == true)
				lastPlatform = dynamic_cast<j1EntityPlatform*>(c2->callback);

			float offset; 

			iPoint shieldPos = GetShieldPos(); 
			iPoint lastShieldPos = GetLastShieldPos(); 
			SDL_Rect shieldRect = GetShieldRect(); 
			SDL_Rect lastShieldRect = GetLastShieldRect(); 

			if (shieldPos.y + shieldRect.h >= c2->rect.y)    // top 
			{
				
				if ((lastShieldPos.y + lastShieldRect.h < c2->rect.y) || onPlatFormType.top)
				{
					ResetPlatformState();
					onPlatFormType.top = true; 
					SetDeviation(true, c2); 
					//LOG("TOP collision !!"); 
				}
	

			}


			if ((shieldPos.y <= c2->rect.y + c2->rect.h) || onPlatFormType.bottom)    // bottom 
			{
				if (lastShieldPos.y > c2->rect.y + c2->rect.h)
				{
					ResetPlatformState();
					onPlatFormType.bottom = true;
					SetDeviation(true, c2);
					//LOG("BOTTOM collision !!");
				}

			}

			if (c2->rect.w > 8000)  // base floor just fucks up next cases
				return;

			if (shieldPos.x + shieldRect.w >= c2->rect.x)    // left 
			{
				if ((lastShieldPos.x + lastShieldRect.w < c2->rect.x) || onPlatFormType.left)
				{
					if (lastShieldPos.y + lastShieldRect.h > c2->rect.y && lastShieldPos.y < c2->rect.y + c2->rect.h)
					{
						ResetPlatformState();
						onPlatFormType.left = true;
						SetDeviation(false, c2);
						//LOG("LEFT collision !!");
					}
				}

			}


			if (shieldPos.x <= c2->rect.x + c2->rect.w)    // right 
			{
				if ((lastShieldPos.x > c2->rect.x + c2->rect.w) || onPlatFormType.right)
				{
					if (lastShieldPos.y + lastShieldRect.h > c2->rect.y && lastShieldPos.y < c2->rect.y + c2->rect.h)
					{
						ResetPlatformState();
						onPlatFormType.right = true;
						SetDeviation(false, c2);
						//LOG("RIGHT collision !!");
					}
				}

			}
		}
		
	}


}

void j1EnemyCacodemon::SetDeviation(bool horizontal, Collider* c2)
{
	if (state.path != ePathState::TEMPORAL_DEVIATION && state.path != ePathState::AWAIT)
	{
		int offset = 3;
		int yOffset = 5;

		if (horizontal)
		{

			if (c2->callback->collider->rect.w > 8000)  // TODO: quickly just ignore base floor 
				return;
			


			if (App->entityFactory->player->position.x + App->entityFactory->player->collider->rect.w / 2 >= position.x + collider->rect.w / 2)   // GO right
				specificDir = iPoint(1, 0);
			
			else
				specificDir = iPoint(-1, 0);
		
	
		}
		else
		{
			if (App->entityFactory->player->position.y + App->entityFactory->player->collider->rect.h / 2 >= position.y + collider->rect.h / 2)   // GO bottom
				specificDir = iPoint(0, 1);
		
			else 
				specificDir = iPoint(0, -1);   // GO top
			
		}


		state.path = ePathState::TEMPORAL_DEVIATION;
	}
	
}

void j1EnemyCacodemon::OnCollisionExit(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_WALL_DETECTION)
	{
		if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL)
		{
			if (keepMovingAfterPlatform == false)
			{
				lastPlatform = nullptr; 
				offPlatformPos = position; 
				keepMovingAfterPlatform = true;
				ResetPlatformState();
			}
		
			
		}
	}

}


