#include "j1EnemyIMP.h"
#include "j1EntityFactory.h"

j1EnemyIMP::j1EnemyIMP(int posX, int posY) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->tex->Load("textures/enemies/IMP/IMP.png");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_IMP;
	size.create(36, 49);
	speed = 150.75f;
	mass = 0.6f;
	gravityFactor = DEFAULT_GRAV * mass;
	tileDetectionRange = 10;

	// jumpdata


	jumpInfo.jumpPower = 15.5f;
	jumpInfo.currenJumpPower = jumpInfo.jumpPower;
	jumpInfo.jumpIncrementFactor = .9f;
	jumpInfo.speedXIncrementJump = 2.3f;
	jumpInfo.speedXIncrementFall = 1.5f;
	jumpInfo.verticalIncrementFactor = 1.02f;
	
	state.movement.at(1) = eMovementState::FALL;   // testing purposes 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 46, 33, size.x, size.y });

	run.PushBack({ 46, 33, size.x, size.y });
	run.PushBack({ 42, 123, size.x + 2, size.y + 2});
	run.PushBack({ 45, 213, size.x -5, size.y + 4});
	run.speed = 1.5f; 
	run.loop = true; 


	death1.PushBack({ 44, 755, size.x + 6, size.y + 13});
	death1.PushBack({ 130, 755, size.x + 5, size.y + 10 });
	death1.PushBack({ 215, 755, size.x + 4, size.y + 5 });
	death1.PushBack({ 299, 755, size.x + 12, size.y - 3 });
	death1.PushBack({ 391, 755, size.x + 18, size.y - 27 });
	death1.speed = 1.3f; 
	death1.loop = false; 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);

}

j1EnemyIMP::~j1EnemyIMP()
{
}

/*
bool j1EnemyIMP::Start()
{

	return true;
}

bool j1EnemyIMP::PreUpdate()
{

	return true;
}

bool j1EnemyIMP::Update(float dt)
{

	return true;
}

bool j1EnemyIMP::PostUpdate()
{

	return true;
}

*/

bool j1EnemyIMP::Move(float dt)
{
	if(j1Enemy::Move(dt))
		Jump();


	return true; 
}


void j1EnemyIMP::Jump()
{
	// modified positions to check offset with platform

	iPoint lastTilePos = App->map->WorldToMap((int)previousPosition.x, (int)position.y) + iPoint(0, 1);
	iPoint tilePos = App->map->WorldToMap((int)position.x, (int)position.y) + iPoint(0, 1);

	if (onPlatform && App->entityFactory->player->onPlatform)  // both on plat 
	{
		bool enoughOffset = false;

		if ((App->entityFactory->player->collider->rect.y + App->entityFactory->player->collider->rect.w < position.y)  // check diff plat height
			|| position.y + collider->rect.y < App->entityFactory->player->position.y)
		{


			for (const auto& col : App->entityFactory->player->collider->onCollisionWithMe)
			{
				if (col->callback->type == ENTITY_DYNAMIC || col->callback->type == ENTITY_STATIC)   // check player in platform 
				{

					iPoint platfTilePos = App->map->WorldToMap((int)col->callback->position.x, (int)col->callback->position.y) + iPoint(0, 1);
					if (pointingDir == RIGHT)
					{
						if (tilePos.x + App->map->WorldToMap(collider->rect.w, 0).x >= platfTilePos.x - jumpTriggerTileRange)               // check I got enough offset to jump and not collide with players' dest platform 
						{
							// check I was coming from that direction 

							if (lastTilePos.x < tilePos.x)
							{
								enoughOffset = true;
							}

						}
					}
					else if (pointingDir == LEFT)
					{
						iPoint platfTilePos = App->map->WorldToMap((int)col->callback->position.x + (int)col->rect.w, (int)col->callback->position.y) + iPoint(0, 1);

						if (tilePos.x <= platfTilePos.x + App->map->WorldToMap(col->rect.w, 0).x + jumpTriggerTileRange)               // check I got enough offset to jump and not collide with players' dest platform 
						{
							if (lastTilePos.x > tilePos.x)
							{
								enoughOffset = true;
							}
						}


					}

				}
			}


			if (enoughOffset)
			{

				doJump = true;

			}


		}
	}

}



