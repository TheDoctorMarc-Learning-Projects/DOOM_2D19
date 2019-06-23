#include "j1EnemyIMP.h"

j1EnemyIMP::j1EnemyIMP(int posX, int posY) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->tex->Load("textures/enemies/IMP/IMP.png");

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_IMP;
	size.create(36, 49);
	speed = .0075f;
	mass = 0.6f;
	gravityFactor = DEFAULT_GRAV * mass;
	

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

bool j1EnemyIMP::PostUpdate()
{

	return true;
}