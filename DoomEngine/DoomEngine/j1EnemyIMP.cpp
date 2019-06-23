#include "j1EnemyIMP.h"

j1EnemyIMP::j1EnemyIMP(int posX, int posY) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->tex->Load("textures/enemies/IMP/IMP.png");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_IMP;
	size.create(36, 49);
	speed = 100.75f;
	mass = 0.6f;
	gravityFactor = DEFAULT_GRAV * mass;
	tileDetectionRange = 10;
	
	state.movement.at(1) = eMovementState::FALL;   // testing purposes 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 46, 33, size.x, size.y });

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);

}

j1EnemyIMP::~j1EnemyIMP()
{
}

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

bool j1EnemyIMP::Move(float dt)
{
	bool move = j1Enemy::Move(dt);
	Jump(); 

	return move; 
}

bool j1EnemyIMP::PostUpdate()
{

	return true;
}


void j1EnemyIMP::Jump()
{
}
