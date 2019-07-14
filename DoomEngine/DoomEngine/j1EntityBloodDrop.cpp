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
	lastPosCollider = collider->rect; 

	if (!floorReached)
	{
		if (!roofReached)
		{
			static float decrementY = 0.7f;
			static float decrementX = 0.97f;

			float v1 = initialSpeed.y *= decrementY;
			float v2 = GravityCalc(gravityFactor, mass) * dt;  // when it has reached roof and has to fall, ONLY this should be considered

			lastSpeed.y = v1 + v2;

			lastSpeed.x *= decrementX;
		}
		else
		{
			initialSpeed.y = 0;  // so that when it falls from roof again, it just goes down with gravity 
			lastSpeed.x = 0;

			if (viscosityData.frameCounter != 0 && viscosityData.frameCounter % viscosityData.frameRate == 0)
				lastSpeed.y = viscosityData.speed;
			else
				lastSpeed.y = 0; 

			viscosityData.frameCounter++; 
		}
			
	
	

		position.x += lastSpeed.x;
		position.y += lastSpeed.y;

		collider->SetPos(position.x, position.y);

		
	}
	else
	{
		if (dynGroundCallback)
		{
			position.y = dynGroundCallback->collider->rect.y - size.y;    // check this out, works just fine, but put it to nullptr

			if(dynGroundCallback->movementType == AXIS_Movement::HORIZONTAL)
				position.x += dynGroundCallback->lastSpeed.x;
		
		}
			
	}
	

	return true;
	
}



void j1EntityBloodDrop::OnCollision(Collider* c1, Collider* c2)
{
	if (!floorReached)
	{
		if (c2->type == COLLIDER_FLOOR)
		{

			float offset = 0.0f; 
			float preventiveOffSet = 20.f; // the collision case when boold goes up, then down in one frame and is inside lower part of platf, not to put it on top of platf
			if (!roofReached)
			{
				if (collider->rect.y + collider->rect.h > c2->rect.y && collider->rect.y + collider->rect.h < c2->rect.y + preventiveOffSet)  // FALL 
				{
					
						if (lastPosCollider.x > c2->rect.x && lastPosCollider.x + lastPosCollider.w < c2->rect.x + c2->rect.w)   // if it comes from top and not sides 
						{
							if (lastSpeed.y > 0)
							{
								floorReached = true;

								offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back if it goes off a bit
								position.y -= offset;


								if (c2->hasCallback && c2->callback->type == ENTITY_DYNAMIC)
									dynGroundCallback = dynamic_cast<j1EntityPlatformDynamic*>(c2->callback); 
							}
						}

					

				}

				if (collider->rect.y < c2->rect.y + c2->rect.h)   // ROOF
				{
					if (lastPosCollider.x > c2->rect.x && lastPosCollider.x + lastPosCollider.w < c2->rect.x + c2->rect.w)   // if it comes from down 
					{
						if (lastSpeed.y <= 0)
						{
							roofReached = true;


							offset = c2->rect.y + c2->rect.h - collider->rect.y;  // to put back if it goes off a bit
							position.y += offset;


						}
					}

				}

				if (collider->rect.x + collider->rect.w > c2->rect.x && lastPosCollider.x + lastPosCollider.w < c2->rect.x && lastSpeed.x > 0) // sides
				{
					roofReached = true;


					offset = collider->rect.x + collider->rect.w - c2->rect.x;  // to put back if it goes off a bit
					position.x -= offset;

				}
				else if (collider->rect.x < c2->rect.x + c2->rect.w && lastPosCollider.x > c2->rect.x + c2->rect.w && lastSpeed.x < 0 )
				{

					roofReached = true;

					offset = c2->rect.x + c2->rect.w - collider->rect.x;  // to put back if it goes off a bit
					position.x += offset;


					
				}


				collider->SetPos(position.x, position.y);
			}
			
		}



		if (c2->type == COLLIDER_WALL)   // just sides 
		{
			float offset = 0.0f;

			if (collider->rect.x + collider->rect.w > c2->rect.x /*&& lastPosCollider.x + lastPosCollider.w < c2->rect.x*/ && lastSpeed.x > 0) // sides
			{
				roofReached = true;


				offset = collider->rect.x + collider->rect.w - c2->rect.x;  // to put back if it goes off a bit
				position.x -= offset;

			}
			else if (collider->rect.x < c2->rect.x + c2->rect.w /*&& lastPosCollider.x > c2->rect.x + c2->rect.w*/ && lastSpeed.x < 0)
			{

				roofReached = true;

				offset = c2->rect.x + c2->rect.w - collider->rect.x;  // to put back if it goes off a bit
				position.x += offset;



			}


			collider->SetPos(position.x, position.y);
		}


	}
	else
	{
		 
		if (c2->type == COLLIDER_BLOOD)     // if a blood drop is in the same pos as another, it will be rendered on top, so no need to have the other alive
		{
			if ((int)position.x == (int)c2->callback->position.x && (int)position.y == (int)c2->callback->position.y)
			{
				c2->callback->to_delete = true; 
			}
				 
		}

		collider->to_delete = true;   // no need to have it anymore; 
		colliderActive = false; 
	}





}



void j1EntityBloodDrop::OnCollisionExit(Collider* c1, Collider* c2)
{
	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL)
	{
		if (lastSpeed.y > 0 && collider->rect.y > c2->rect.y + c2->rect.h)
		{
			if (roofReached)
			{
				roofReached = false;

				if (c2->type == COLLIDER_WALL)
				{
					floorReached = true;
					float offset = collider->rect.y + collider->rect.h - c2->rect.y;  // to put back if it goes off a bit
					position.y -= offset;
				}// when it is sliding down a wall and gets to the botton, we must assume its floor 
					 
			}
				
		}
	
	}
}


bool j1EntityBloodDrop::CleanUp()
{
	j1Entity::CleanUp(); 

	if (dynGroundCallback)
		dynGroundCallback = nullptr; 

	return true; 
}
