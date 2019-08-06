#include "j1EnemyBaronOfHell.h" 
#include "j1EntityFactory.h"

j1EnemyBaronOfHell::j1EnemyBaronOfHell(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_BARON_OF_HELL;
	this->name = name;
	powerLevel = 4U;
	maxLife = 500;
	life = (float)maxLife;
	size.create(44, 70);
	speed = 80.33f;
	defaultSpeed = speed; 
	mass = 1.3f;
	gravityFactor = DEFAULT_GRAV / mass;  // TODO: just prevent Y movement by gravity
	tileDetectionRange = 15;
	cadenceValues.melee = 2500;
	cadenceValues.longRange = 2500;
	damageValues.melee = 120;
	damageValues.longRange = 60;
	longRangeShootData.msWaitFromAnimStartToShot = 250;
	longRangeShootData.relativeOffsetPos.create(0, 26); // assuming spritescale is 1.0f
	longRangeShootData.shotSpeed = 350;
	pathType = enemyPathType::A_TO_B;
	dataAnimFx.hasSecondDeathAnim = false;
	dataAnimFx.hasSecondDeathFx = false;
	dataAnimFx.hasSecondAttackFx = true; 
	hasMaxDistanceFromOrigin = true;
	maxDistFromOrigin = 10U;
	myMeleeRange = 2U;
	hasMaxDistanceFromOrigin = false;

	state.movement.at(1) = eMovementState::FALL;    // check this out: when TODO: load enemies in map, it should no longer be fall  

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 20, 114, size.x, size.y });

	run.PushBack({ 13, 8, size.x + 14, size.y - 1});
	run.PushBack({ 20, 114, size.x, size.y });
	run.PushBack({ 23, 221, size.x + 6, size.y - 1});
	run.PushBack({ 27, 328, size.x, size.y + 2});
	// missing speed: Baron and Knight

	death1.PushBack({ 44, 878, size.x + 4, size.y + 3});
	death1.PushBack({ 136, 878, size.x + 11, size.y - 6});
	death1.PushBack({ 235, 878, size.x + 12, size.y - 16});
	death1.PushBack({ 335, 878, size.x + 15, size.y - 34});
	death1.PushBack({ 438, 878, size.x + 16, size.y - 40});
	death1.PushBack({ 542, 878, size.x + 16, size.y - 40});
	death1.PushBack({ 646, 878, size.x + 16, size.y - 40});
	death1.speed = 2.2f;
	death1.loop = false;


	attack.PushBack({ 29, 435, size.x + 12, size.y - 4});
	attack.PushBack({ 29, 544, size.x + 7, size.y - 2});
	attack.PushBack({ 36, 654, size.x + 5, size.y - 8});
	attack.PushBack({ 35, 751, size.x + 3, size.y + 1});
	attack.speed = 2.7f;
	attack.loop = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);
}

j1EnemyBaronOfHell::~j1EnemyBaronOfHell()
{
}


bool j1EnemyBaronOfHell::Move(float dt)
{
	if (App->entityFactory->IsPlayerAlive() == false) // first line prevention _> TODO: maybe it is safer to just stop the entity factory, but the player weapon wouldn't fall to the floor then 
	{
		return false;

	}
	j1Enemy::Move(dt); 
	
	if (state.combat != eCombatState::DYING && state.combat != eCombatState::DEAD)
	{
		if (isPlayerOnMeleeRange() == true && state.combat != eCombatState::SHOOT && lastSpeed.IsZero() == true)
			currentAnimation = &idle;

		if(App->entityFactory->player->onPlatform == false || (App->entityFactory->player->onPlatform == true && lastPlatform != App->entityFactory->player->lastPlatform))
			Go_A_to_B();

		DoMeleeAttack();

		if (LongRangeConditions() == true)
			DoLongRangeAttack();
		else
			if(state.combat == eCombatState::SHOOT && currentAttackType == ATTACK_TYPE::LONG_RANGE)     // if conditions are not met, reset attack data
				if(currentAnimation->Finished() == true)
					PutCombatStateToIdle();
			              
			

	}


	

	return true;
}

bool j1EnemyBaronOfHell::LongRangeConditions()
{

	// no walls between player and myself :) 

	if (j1Enemy::isWallBetweenPlayerAndMe() == true)
		return false;


	// player on my collider height horizontal projection AND in the shot projection 

	if (App->entityFactory->player->position.y + App->entityFactory->player->collider->rect.h < position.y + longRangeShootData.relativeOffsetPos.y)
		return false; 
	if (App->entityFactory->player->position.y > position.y + collider->rect.h - (collider->rect.h - longRangeShootData.relativeOffsetPos.y))
		return false; 


	return true; 
}

fPoint j1EnemyBaronOfHell::GetShotSpeed(fPoint dir) const
{
	fPoint speed = fPoint(0, 0); 
	speed.x = longRangeShootData.shotSpeed;

	if(dir.x < 0)
		speed.x = -speed.x; 
		
	return speed; 
}




void j1EnemyBaronOfHell::ResolvePathDeviation()
{
	if (pointingDir == LEFT)
		pointingDir = RIGHT;
	else if (pointingDir == RIGHT)
		pointingDir = LEFT; 


	speed = defaultSpeed; 
}

 