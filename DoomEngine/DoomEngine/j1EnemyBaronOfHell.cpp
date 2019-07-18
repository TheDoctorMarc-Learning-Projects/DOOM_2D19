#include "j1EnemyBaronOfHell.h" 
#include "j1EntityFactory.h"

j1EnemyBaronOfHell::j1EnemyBaronOfHell(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_BARON_OF_HELL;
	this->name = name;
	maxLife = 500;
	life = (float)maxLife;
	size.create(44, 70);
	speed = 80.33f;
	mass = 1.3f;
	gravityFactor = DEFAULT_GRAV / mass;  // TODO: just prevent Y movement by gravity
	tileDetectionRange = 15;
	cadenceValues.melee = 2500;
	cadenceValues.longRange = 2500;
	damageValues.melee = 120;
	damageValues.longRange = 40;
	longRangeShootData.msWaitFromAnimStartToShot = 300;
	longRangeShootData.relativeOffsetPos.create(0, 26); // assuming spritescale is 1.0f
	longRangeShootData.shotSpeed = 250;
	pathType = enemyPathType::A_TO_B;
	dataAnimFx.hasSecondDeathAnim = false;
	dataAnimFx.hasSecondDeathFx = false;
	dataAnimFx.hasSecondAttackFx = true; 
	hasMaxDistanceFromOrigin = true;
	maxDistFromOrigin = 10U;
	myMeleeRange = 2U;

	state.movement.at(1) = eMovementState::FALL;    // check this out: when TODO: load enemies in map, it should no longer be fall  

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 20, 114, size.x, size.y });

	run.PushBack({ 13, 8, size.x + 14, size.y - 1});
	run.PushBack({ 20, 114, size.x, size.y });
	run.PushBack({ 23, 221, size.x + 6, size.y - 1});
	run.PushBack({ 27, 328, size.x, size.y + 2});

	death1.PushBack({ 44, 878, size.x + 4, size.y + 3});
	death1.PushBack({ 136, 878, size.x + 11, size.y - 6});
	death1.PushBack({ 235, 878, size.x + 12, size.y - 16});
	death1.PushBack({ 335, 878, size.x + 15, size.y - 34});
	death1.PushBack({ 438, 878, size.x + 16, size.y - 40});
	death1.PushBack({ 542, 878, size.x + 16, size.y - 40});
	death1.PushBack({ 646, 878, size.x + 16, size.y - 40});
	death1.speed = 1.7f;
	death1.loop = false;


	attack.PushBack({ 29, 435, size.x + 12, size.y - 4});
	attack.PushBack({ 29, 544, size.x + 7, size.y - 2});
	attack.PushBack({ 36, 654, size.x + 5, size.y - 8});
	attack.PushBack({ 35, 751, size.x + 3, size.y + 1});
	attack.speed = 2.2f;
	attack.loop = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);
}

j1EnemyBaronOfHell::~j1EnemyBaronOfHell()
{
}


bool j1EnemyBaronOfHell::Move(float dt)
{
	if (j1Enemy::Move(dt))
	{
	}

	DoMeleeAttack();
//	DoLongRangeAttack();


	// todo: when shooting, stop moving 
	// todo: shoot only if i'm on camera (do it on cacodemon too) 


	return true;
}