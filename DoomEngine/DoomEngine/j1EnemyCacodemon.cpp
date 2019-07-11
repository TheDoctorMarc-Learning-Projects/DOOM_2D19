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
	death1.speed = 1.3f;
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

}

j1EnemyCacodemon::~j1EnemyCacodemon()
{
}


bool j1EnemyCacodemon::Move(float dt)
{
	if (j1Enemy::Move(dt))
	{
	}



	return true;
}
