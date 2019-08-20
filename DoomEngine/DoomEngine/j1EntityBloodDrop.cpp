#include "j1EntityBloodDrop.h"
#include "j1EntityFactory.h"
#include "j1BloodManager.h"
#include "j1Entity.h"


j1EntityBloodDrop::j1EntityBloodDrop(int posX, int posY, fPoint Speed, Color c) : j1Entity(posX, posY)
{
	drawActive = false; 
	mass = 1.F; 
	gravityFactor = DEFAULT_GRAV / 1;
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

	if (collider && collider->type != COLLIDER_BLOOD)
		LOG("Blood has got a collider that it should not. Fatal Error");


	App->render->DrawQuad({ (int)position.x, (int)position.y, size.x, size.y }, c.r, c.g, c.b, c.a, true);


}

bool j1EntityBloodDrop::Update(float dt)
{
	if (colliderActive == false)
		return false; 

	lastPosCollider = collider->rect;

	if (!floorReached)
	{
		if (!roofReached)
		{
			float v1 = initialSpeed.y *= speedDecrement.y;
			float v2 = GravityCalc(gravityFactor, mass) * dt;  // when it has reached roof and has to fall, ONLY this should be considered

			lastSpeed.y = v1 + v2;

			lastSpeed.x *= speedDecrement.x;
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

			if (dynGroundCallback->movementType == AXIS_Movement::HORIZONTAL)
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
				else if (collider->rect.x < c2->rect.x + c2->rect.w && lastPosCollider.x > c2->rect.x + c2->rect.w && lastSpeed.x < 0)
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

		if (colliderActive == true)
		{
			collider->to_delete = true;   // no need to have it anymore; 
			colliderActive = false;
		}
		
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


bool j1EntityBloodDrop::Save(pugi::xml_node& node) const
{
	// GENERAL DATA --> simialr to entity
	node.append_child("name").append_attribute("value") = name.c_str();

	auto pos = node.append_child("position");
	pos.append_attribute("x") = position.x;
	pos.append_attribute("y") = position.y;

	auto lSpeed = node.append_child("last_speed");
	lSpeed.append_attribute("x") = lastSpeed.x;
	lSpeed.append_attribute("y") = lastSpeed.y;

	
	node.append_child("collider_active").append_attribute("value") = colliderActive;
	node.append_child("to_delete").append_attribute("value") = to_delete;


	// PARTICULAR DATA

	auto colorNode = node.append_child("color");
	colorNode.append_attribute("R") = c.r; 
	colorNode.append_attribute("G") = c.g;
	colorNode.append_attribute("B") = c.b;
	colorNode.append_attribute("A") = c.a;

	node.append_child("floor_reached").append_attribute("value") = floorReached; 
	node.append_child("roof_reached").append_attribute("value") = roofReached;

	auto lastPosColNode = node.append_child("last_position_collider");
	lastPosColNode.append_attribute("x") = lastPosCollider.x; 
	lastPosColNode.append_attribute("y") = lastPosCollider.y;
	lastPosColNode.append_attribute("w") = lastPosCollider.w;
	lastPosColNode.append_attribute("h") = lastPosCollider.h;

	node.append_child("viscosity_actual_frame").append_attribute("value") = viscosityData.frameCounter; 

	auto entitiesNode = node.append_child("related_entities_node");
	auto dynGroundNode = entitiesNode.append_child("dynamic_ground_callback"); 

	if (dynGroundCallback != nullptr)
	{
		dynGroundNode.append_child("has_dynamic_ground_callback").append_attribute("value") = true;
		dynGroundNode.append_child("dynamic_ground_ID").append_attribute("value") = dynGroundCallback->ID;
	}
	else
		dynGroundNode.append_child("has_dynamic_ground_callback").append_attribute("value") = false; 



	return true;
}
