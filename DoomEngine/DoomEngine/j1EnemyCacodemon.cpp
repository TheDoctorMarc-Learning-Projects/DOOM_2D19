#include "j1EnemyCacodemon.h" 
#include "j1EntityFactory.h"

j1EnemyCacodemon::j1EnemyCacodemon(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_CACODEMON;
	this->name = name;
	damage = 50;
	maxLife = 300;
	life = (float)maxLife;
	size.create(62, 67);
	speed = 50.75f;
	mass = 11.f;
	gravityFactor = DEFAULT_GRAV * mass;  // TODO: just prevent Y movement by gravity
	tileDetectionRange = 15;
	cadenceValues.melee = 1200;
	cadenceValues.longRange = 1000; 
	pathType = enemyPathType::FLYING;
	deathDataAnimFx.hasSecondDeathAnim = false;
	deathDataAnimFx.hasSecondDeathFx = false;

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
	death1.speed = 4.7f;
	death1.loop = false;


	attack.PushBack({ 34, 126, size.x, size.y });
	attack.PushBack({ 33, 226, size.x, size.y + 2});
	attack.PushBack({ 34, 328, size.x, size.y + 8});
	attack.PushBack({ 35, 436, size.x, size.y + 1});
	attack.PushBack({ 35, 537, size.x, size.y + 1});
	attack.speed = 2.5f;
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



	return true;
}

void j1EnemyCacodemon::KeepMovingTendency()
{
	  // if pos has advanced some units, stop moving tendency
	    specificDir = iPoint(0, 0); 
		state.path = ePathState::FOLLOW_PLAYER;
		keepMovingAfterPlatform = false; 
}


bool j1EnemyCacodemon::CleanUp()
{
	j1Entity::CleanUp(); 

	// clean the area collider: 
	shieldAreaCollider->to_delete = true; 

	return true; 
}


void j1EnemyCacodemon::OnCollision(Collider* c1, Collider* c2)
{
	if (c1->type == COLLIDER_WALL_DETECTION)
	{
		if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL)
		{
			iPoint shieldPos = GetShieldPos(); 
			iPoint lastShieldPos = GetLastShieldPos(); 
			SDL_Rect shieldRect = GetShieldRect(); 
			SDL_Rect lastShieldRect = GetLastShieldRect(); 

			if (shieldPos.y + shieldRect.h >= c2->callback->position.y)    // top 
			{
				if ((lastShieldPos.y + lastShieldRect.h < c2->callback->position.y) || onPlatFormType.top)
				{
					ResetPlatformState();
					onPlatFormType.top = true; 
					SetDeviation(true, c2); 
					LOG("TOP collision !!"); 
				}

			}


			if ((shieldPos.y <= c2->callback->position.y + c2->rect.h) || onPlatFormType.bottom)    // bottom 
			{
				if (lastShieldPos.y > c2->callback->position.y + c2->rect.h)
				{
					ResetPlatformState();
					onPlatFormType.bottom = true;
					SetDeviation(true, c2);
					LOG("BOTTOM collision !!");
				}

			}

			if (c2->callback->collider->rect.w > 8000)  // base floor just fucks up next cases
				return; 

			if (shieldPos.x + shieldRect.w >= c2->callback->position.x)    // left 
			{
				if ((lastShieldPos.x + lastShieldRect.w < c2->callback->position.x) || onPlatFormType.left)
				{
					if (lastShieldPos.y + lastShieldRect.h > c2->callback->position.y && lastShieldPos.y < c2->callback->position.y + c2->rect.h)
					{
						ResetPlatformState();
						onPlatFormType.left = true;
						SetDeviation(false, c2);
						LOG("LEFT collision !!");
					}
				}

			}


			if (shieldPos.x <= c2->callback->position.x + c2->rect.w)    // right 
			{
				if ((lastShieldPos.x > c2->callback->position.x + c2->rect.w) || onPlatFormType.right)
				{
					if (lastShieldPos.y + lastShieldRect.h > c2->callback->position.y && lastShieldPos.y < c2->callback->position.y + c2->rect.h)
					{
						ResetPlatformState();
						onPlatFormType.right = true;
						SetDeviation(false, c2);
						LOG("RIGHT collision !!");
					}
				}

			}
		}
		
	}


}

void j1EnemyCacodemon::SetDeviation(bool horizontal, Collider* c2)
{
	if (state.path != ePathState::TEMPORAL_DEVIATION)
	{
		int offset = 3;
		int yOffset = 5;

		if (horizontal)
		{

			/*if (c2->callback->collider->rect.w > 8000)  // TODO: quickly just ignore base floor 
			{*/
				/*position.y = c2->callback->collider->rect.y - GetShieldRect().h;
				collider->SetPos(position.x, position.y);

				shieldAreaCollider->SetPos(position.x - shieldExtraSideSize / 2, position.y - shieldExtraSideSize / 2);*/
			/*	return;
			}*/


			if (App->entityFactory->player->position.x + App->entityFactory->player->collider->rect.w / 2 >= position.x + collider->rect.w / 2)   // GO right 
			{
				//targetPos.value.x = App->map->WorldToMap(c2->callback->position.x + c2->callback->collider->rect.w, 0).x + offset;
				specificDir = iPoint(1, 0); 
			}
			else
			{
				//targetPos.value.x = App->map->WorldToMap(c2->callback->position.x - this->collider->rect.w, 0).x - offset;    // GO left
				specificDir = iPoint(-1, 0);
			}
			/*targetPos.value.y = App->map->WorldToMap(0, position.y).y;

			targetPos.type = TargetPos::targetPosType::X;*/
		}
		else
		{
			if (App->entityFactory->player->position.y + App->entityFactory->player->collider->rect.h / 2 >= position.y + collider->rect.h / 2)
			{
				//targetPos.value.y = App->map->WorldToMap(0, c2->callback->position.y + c2->callback->collider->rect.h).y + yOffset;    // GO down 
				specificDir = iPoint(0, 1);
			}
			else
			{
				//targetPos.value.y = App->map->WorldToMap(0, c2->callback->position.y).y - yOffset;    // GO top
				specificDir = iPoint(0, -1);
			}

				//targetPos.value.x = App->map->WorldToMap(position.x, 0).x; 
			
			


			//targetPos.type = TargetPos::targetPosType::Y;
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
				keepMovingAfterPlatform = true;
				ResetPlatformState();
			}
		
			
		}
	}

}


