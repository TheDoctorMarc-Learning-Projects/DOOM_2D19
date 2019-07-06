#include "j1EntityBloodDrop.h"
#include "j1EntityFactory.h"
#include "j1BloodManager.h"



j1EntityBloodDrop::j1EntityBloodDrop(int posX, int posY, fPoint Speed, Color c) : j1Entity(posX, posY)
{

	drawActive = false; 
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
	collider->SetPos(position.x, position.y); 

}

bool j1EntityBloodDrop::Update(float dt)
{
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
