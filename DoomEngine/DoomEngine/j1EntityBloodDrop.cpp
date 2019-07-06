#include "j1EntityBloodDrop.h"
#include "j1EntityFactory.h"
#include "j1BloodManager.h"



j1EntityBloodDrop::j1EntityBloodDrop(int posX, int posY, fPoint Speed, Color c) : j1Entity(posX, posY)
{
	
	mass = 1.F; 
	gravityFactor = DEFAULT_GRAV * mass;
	initialSpeed = Speed; 
	lastSpeed = Speed; 
	this->c = c; 
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = BLOOD; // update this in dynamic platforms

	position.x = posX;
	position.y = posY;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ posX, posY, bloodCubeSideSize, bloodCubeSideSize }, COLLIDER_TYPE::COLLIDER_BLOOD, this);
	size.create(bloodCubeSideSize, bloodCubeSideSize); 

}

j1EntityBloodDrop::~j1EntityBloodDrop()
{
}


void j1EntityBloodDrop::Draw()
{
	
	App->render->DrawQuad(collider->rect, c.r, c.g, c.b, c.a, true); 


}

bool j1EntityBloodDrop::Update(float dt)
{
	static float decrementY = 0.85f; 
	static float decrementX = 0.93f; 

	

		float v1 = initialSpeed.y *= decrementY;
		float v2 = GravityCalc(gravityFactor, mass) * dt;

		lastSpeed.y = v1 + v2;
	
	lastSpeed.x *= decrementX; 

	position.x += lastSpeed.x; 
	position.y += lastSpeed.y; 

	collider->SetPos(position.x, position.y);

	return true; 
}

bool j1EntityBloodDrop::CleanUp()
{
	collider->to_delete = true; 

	return true; 
}


void j1EntityBloodDrop::OnCollision(Collider* c1, Collider* c2)
{

}



/*void j1EntityBloodDrop::OnCollisionExit(Collider* c1, Collider* c2)
{

}*/
