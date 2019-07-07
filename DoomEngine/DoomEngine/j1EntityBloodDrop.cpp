#include "j1EntityBloodDrop.h"
#include "j1EntityFactory.h"
#include "j1BloodManager.h"



j1EntityBloodDrop::j1EntityBloodDrop(int posX, int posY, fPoint Speed, Color c) : j1Entity(posX, posY)
{
	drawActive = false; 
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
	
	App->render->DrawQuad({(int)position.x, (int)position.y, size.x, size.y}, c.r, c.g, c.b, c.a, true);


}

bool j1EntityBloodDrop::Update(float dt)
{
	if (!floorReached)
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

		
	}

	return true;
	
}



void j1EntityBloodDrop::OnCollision(Collider* c1, Collider* c2)
{
	if (!floorReached)
	{
		if (c2->type == COLLIDER_FLOOR)
		{
			if (collider->rect.y + collider->rect.h > c2->rect.y)
			{
				if (lastSpeed.y > 0)
				{
					floorReached = true;

					float offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back if it goes off a bit
					position.y -= offset;


					collider->SetPos(position.x, position.y);

				}
				

			}
		}


	}
	else
	{
		 
		if (c2->type == COLLIDER_BLOOD)     // if a blood drop is EXACTLY in the same pos as another, it will be rendered on top, so no need to have the other alive
		{
			if (position.x == c2->callback->position.x && position.y == c2->callback->position.y)
			{
				c2->callback->to_delete = true; 
			}
				 
		}

		collider->to_delete = true;   // no need to have it anymore; 
		colliderActive = false; 
	}


}



/*void j1EntityBloodDrop::OnCollisionExit(Collider* c1, Collider* c2)
{

}*/
