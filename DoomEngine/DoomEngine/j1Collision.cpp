#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "p2Log.h"
#include "j1Particles.h"
#include "j1EntityFactory.h"
#include "Brofiler/Brofiler.h"
#include "Color.h"
#include "j1Entity.h"




j1Collision::j1Collision()
{
	name.assign("collision");

	matrix[COLLIDER_PRESENTIAL][COLLIDER_PRESENTIAL] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_LOOT] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_WALL] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_GOD] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_WIN] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_SHOT] = false;
	matrix[COLLIDER_PRESENTIAL][COLLIDER_FLOOR] = false;



	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_LOOT] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_DEATH] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_GOD] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_WIN] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_SHOT] = false;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY_SHOT][COLLIDER_PRESENTIAL] = false;


	matrix[COLLIDER_WALL_DETECTION][COLLIDER_WALL_DETECTION] = true;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_LOOT] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_WALL] = true;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_GOD] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_WIN] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_SHOT] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_WALL_DETECTION][COLLIDER_PRESENTIAL] = false;


	matrix[COLLIDER_BLOOD][COLLIDER_BLOOD] = true;
	matrix[COLLIDER_BLOOD][COLLIDER_LOOT] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_WALL] = true;
	matrix[COLLIDER_BLOOD][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_DEATH] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_GOD] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_WIN] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_SHOT] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_BLOOD][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_BLOOD][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_LOOT][COLLIDER_LOOT] = false;
	matrix[COLLIDER_LOOT][COLLIDER_WALL] = false;
	matrix[COLLIDER_LOOT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_LOOT][COLLIDER_DEATH] = false;
	matrix[COLLIDER_LOOT][COLLIDER_GOD] = true;
	matrix[COLLIDER_LOOT][COLLIDER_WIN] = false;
	matrix[COLLIDER_LOOT][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_LOOT][COLLIDER_SHOT] = false;
	matrix[COLLIDER_LOOT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_LOOT][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_LOOT][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_LOOT][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_LOOT][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL][COLLIDER_GOD] = true;
	matrix[COLLIDER_WALL][COLLIDER_WIN] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_SHOT] = true;
	matrix[COLLIDER_WALL][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_WALL][COLLIDER_LOOT] = false;
	matrix[COLLIDER_WALL][COLLIDER_BLOOD] = true;
	matrix[COLLIDER_WALL][COLLIDER_WALL_DETECTION] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY_SHOT] = true;
	matrix[COLLIDER_WALL][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = true;  // needed for dynamic platforms
	matrix[COLLIDER_FLOOR][COLLIDER_WALL] = false; 
	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_DEATH] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_GOD] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_WIN] = false;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_SHOT] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_LOOT] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_BLOOD] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_WALL_DETECTION] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY_SHOT] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SHOT] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_LOOT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY_SHOT] = true;     
 	matrix[COLLIDER_PLAYER][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH][COLLIDER_GOD] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WIN] = false;
	matrix[COLLIDER_DEATH][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_DEATH][COLLIDER_SHOT] = false;
	matrix[COLLIDER_DEATH][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_DEATH][COLLIDER_LOOT] = false;
	matrix[COLLIDER_DEATH][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_DEATH][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_DEATH][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_GOD][COLLIDER_GOD] = false;
	matrix[COLLIDER_GOD][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_GOD][COLLIDER_WALL] = true;
	matrix[COLLIDER_GOD][COLLIDER_DEATH] = true;
	matrix[COLLIDER_GOD][COLLIDER_WIN] = true;
	matrix[COLLIDER_GOD][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_GOD][COLLIDER_SHOT] = true;
	matrix[COLLIDER_GOD][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_GOD][COLLIDER_LOOT] = true;
	matrix[COLLIDER_GOD][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_GOD][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_GOD][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_GOD][COLLIDER_PRESENTIAL] = false;


	matrix[COLLIDER_WIN][COLLIDER_WIN] = false;
	matrix[COLLIDER_WIN][COLLIDER_GOD] = true;
	matrix[COLLIDER_WIN][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WIN][COLLIDER_WALL] = false;
	matrix[COLLIDER_WIN][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WIN][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_WIN][COLLIDER_SHOT] = false;
	matrix[COLLIDER_WIN][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_WIN][COLLIDER_LOOT] = false;
	matrix[COLLIDER_WIN][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_WIN][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_WIN][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_WIN][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WIN] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_GOD] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_SHOT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_LOOT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_PRESENTIAL] = false;

	matrix[COLLIDER_SHOT][COLLIDER_SHOT] = false;
	matrix[COLLIDER_SHOT][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_SHOT][COLLIDER_WIN] = false;
	matrix[COLLIDER_SHOT][COLLIDER_DEATH] = false;
	matrix[COLLIDER_SHOT][COLLIDER_GOD] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WALL] = true;
	matrix[COLLIDER_SHOT][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_SHOT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_SHOT][COLLIDER_LOOT] = false; 
	matrix[COLLIDER_SHOT][COLLIDER_BLOOD] = false;
	matrix[COLLIDER_SHOT][COLLIDER_WALL_DETECTION] = false;
	matrix[COLLIDER_SHOT][COLLIDER_ENEMY_SHOT] = false;
	matrix[COLLIDER_SHOT][COLLIDER_PRESENTIAL] = false;
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	BROFILER_CATEGORY("Collision PreUpdate", Profiler::Color::DarkSeaGreen);

	if (App->pause == true)
		return true;

	bool ret = true; 
	// Remove all colliders scheduled for deletion
	for (auto collider = colliders.begin(); collider != colliders.end();)
	{
		if ((*collider)->to_delete == false)
		{
			(*collider)->UpdateStatus();
			++collider;
		}

		else
		{
			(*collider)->FreeOnCollision();
			RELEASE(*collider);
			collider = colliders.erase(collider);

		}
			
	}

	// Calculate collisions

	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < colliders.size(); ++i)
	{
		c1 = colliders.at(i);

		// avoid checking collisions already checked
		for (uint k = i + 1; k < colliders.size(); ++k)
		{
			c2 = colliders.at(k);
			
			if (c1->CheckCollision(c2->rect) == false)
			{
				// useful when colliders "exit" a collision
				if (matrix[c1->type][c2->type] && c1->callback)
					doCollisionDeAssignment(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					doCollisionDeAssignment(c2, c1);

			}
			else     
			{

				if (matrix[c1->type][c2->type] && c1->callback)
				{
					c1->callback->OnCollision(c1, c2);
					doCollisionAssignment(c1, c2);
				}

				if (matrix[c2->type][c1->type] && c2->callback)
				{
					c2->callback->OnCollision(c2, c1);
					doCollisionAssignment(c2, c1);
				}

			}
		}
	}


	return ret;

}

void j1Collision::doCollisionAssignment(Collider* c1, Collider* c2)
{
	for (const auto& col : c1->onCollisionWithMe)
		if (col == c2)
			return;

	c1->onCollisionWithMe.push_back(c2);
}

void j1Collision::doCollisionDeAssignment(Collider* c1, Collider* c2)
{

	if (!c1->onCollisionWithMe.empty())
	{
		for (auto col = c1->onCollisionWithMe.begin(); col != c1->onCollisionWithMe.end();)
		{
			if ((*col) == c2)
			{
				col = c1->onCollisionWithMe.erase(col);
				c1->callback->OnCollisionExit(c1, c2);
				break; 
			}

			else
				++col;

		}

	}

}


// Called before render is available
bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Collision Update", Profiler::Color::DarkSlateBlue);
	bool ret = true;
	
	if (App->pause == true)
		return true;

	if (debug)
		DebugDraw();

	return ret;
}

void j1Collision::DebugDraw()
{
	BROFILER_CATEGORY("Collision DebugDraw", Profiler::Color::DarkSlateGray);
	
	Uint8 alpha = 80;
	for (const auto& collider : colliders)         // TDOO: draw only if on camera
	{
		if (collider == nullptr)
			continue;

		switch (collider->type)
		{

		case COLLIDER_NONE: // white
			App->render->DrawQuad(collider->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL: // red
			App->render->DrawQuad(collider->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(collider->rect, 0, 255, 0, alpha);
			break;
		/*case COLLIDER_DEATH: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
			break;*/
		case COLLIDER_GOD: // violet
			App->render->DrawQuad(collider->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_WIN: 
			App->render->DrawQuad(collider->rect, 255, 150, 255, alpha);
			break;
		case COLLIDER_ENEMY:
			App->render->DrawQuad(collider->rect, 255, 50, 255, alpha);
			break;
		case COLLIDER_SHOT:
			App->render->DrawQuad(collider->rect, 0, 0, 0, alpha);
			break;
		case COLLIDER_FLOOR:
			App->render->DrawQuad(collider->rect, 50, 50, 255, alpha);
			break;
		case COLLIDER_LOOT:
			App->render->DrawQuad(collider->rect, 150, 230, 200, alpha);
			break;
		case COLLIDER_WALL_DETECTION:
			App->render->DrawQuad(collider->rect, 0, 30, 30, alpha);
			break;
		case COLLIDER_ENEMY_SHOT:
			App->render->DrawQuad(collider->rect, 110, 170, 200, alpha);
			break;
		case COLLIDER_PRESENTIAL:
			App->render->DrawQuad(collider->rect, 25, 70, 20, alpha);
			break;

		}
	}
}

// Called before quitting
bool j1Collision::CleanUp()
{
	BROFILER_CATEGORY("Collision CleanUp", Profiler::Color::DarkTurquoise);
	bool ret = true;
	LOG("Freeing all colliders");

	for (auto& collider : colliders)
	{
		collider->FreeOnCollision();
		RELEASE(collider);
	}
	colliders.clear(); 

	return ret; 
}
                                                 

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback, fPoint speed, bool volatileOutOfScreen)
{
	BROFILER_CATEGORY("Collision AddCollider", Profiler::Color::DeepPink);
	Collider* ret = nullptr;

	ret = DBG_NEW Collider(rect, type, callback, speed, volatileOutOfScreen);
	colliders.push_back(ret); 

	return ret;
}

void j1Collision::DestroyCollider(Collider* col)
{
	col->to_delete = true; 

	/*col->FreeOnCollision();
	RELEASE(col);
	colliders.erase(std::remove(colliders.begin(), colliders.end(), col), colliders.end());*/
}


bool j1Collision::Load(pugi::xml_node& node) 
{
	for (auto colNode = node.child("collider_player_shot"); colNode; colNode = colNode.next_sibling())
	{
		SDL_Rect colRect = { 0,0,0,0 }; 
		colRect.x = colNode.child("rect").attribute("x").as_int(); 
		colRect.y = colNode.child("rect").attribute("y").as_int();
		colRect.w = colNode.child("rect").attribute("w").as_int();
		colRect.h = colNode.child("rect").attribute("h").as_int();

		j1Entity* callback; 
		if (colNode.child("callback_node").attribute("exists").as_bool() == true)
		{
			uint ID = colNode.child("callback_node").child("weapon_callback_ID").attribute("value").as_uint();
			callback = App->entityFactory->GetEntityFromID(ID);
		}
		else
			callback = nullptr; 

		fPoint speed = fPoint(colNode.child("speed").attribute("x").as_float(), colNode.child("speed").attribute("y").as_float()); 
		fPoint initialPos = fPoint(colNode.child("initial_pos").attribute("x").as_float(), colNode.child("initial_pos").attribute("y").as_float());

		AddCollider(colRect, COLLIDER_SHOT, callback, speed, true); 
	}


	return true;
}

bool j1Collision::Save(pugi::xml_node& node) const
{
	for(const auto& col : colliders)
		if (col->type == COLLIDER_SHOT)
		{
			auto colNode = node.append_child("collider_player_shot"); 
			auto rectNode = colNode.append_child("rect"); 
			rectNode.append_attribute("x") = col->rect.x;
			rectNode.append_attribute("y") = col->rect.y;
			rectNode.append_attribute("w") = col->rect.w;
			rectNode.append_attribute("h") = col->rect.h;
			colNode.append_child("to_delete").append_attribute("value") = col->to_delete; 
			auto callbackNode = colNode.append_child("callback_node"); 
			if (col->callback != nullptr)
			{
				callbackNode.append_attribute("exists") = true; 
				callbackNode.append_child("weapon_callback_ID").append_attribute("value") = col->callback->ID; 
			}
			else
				callbackNode.append_attribute("exists") = false;
			auto speedNode = colNode.append_child("speed"); 
			speedNode.append_attribute("x") = col->speed.x; 
			speedNode.append_attribute("y") = col->speed.y;

			auto initialPosNode = colNode.append_child("initial_pos");
			initialPosNode.append_attribute("x") = col->initialPos.x;
			initialPosNode.append_attribute("y") = col->initialPos.y;

		}

	return true; 
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) 
{
	BROFILER_CATEGORY("Collision CheckCollision", Profiler::Color::DeepSkyBlue);
	bool ret = true;

	if (r.x + r.w < rect.x) ret = false;
	else if (r.x > rect.x + rect.w) ret = false;
	else if (r.y + r.h < rect.y)ret = false;
	else if (r.y > rect.h + rect.y)ret = false;

	return ret;
}

void Collider::FreeOnCollision()
{
	if (onCollisionWithMe.empty() == false)
	{
		for (auto col = onCollisionWithMe.begin(); col != onCollisionWithMe.end();)    // go through my oncollisionlist...
		{
			bool iWasDeleted = false;

			if ((*col)->onCollisionWithMe.empty() == false) 
			{
				for (auto colReverse = (*col)->onCollisionWithMe.begin(); colReverse != (*col)->onCollisionWithMe.end();)  // my oncollision list has colliders, and those colliders have their own list, where I CAN be 
				{
					if ((*colReverse) == this)  // is I find my self in another collider on collision with me list, delete myself from the list
					{
						colReverse = (*col)->onCollisionWithMe.erase(colReverse);
						iWasDeleted = true;
					}
					else
					{
						++colReverse;
					}
				}
			}

			if (iWasDeleted == true)  // if I have been deleted from the other's list, then delete the other from my list too 
				col = onCollisionWithMe.erase(col);
			else
				++col;
		
		}

	}
}


void Collider::UpdateStatus()
{
	if (volatileOutOfScreen)  // delete whe out of camera limits
	{

		if (App->render->isRectOnCamera(this->rect) == false)              // TODO: do this rather with is on camera ?
		{
			if (owner != nullptr)
				owner->to_delete = true;   // if it has an owner, delete the owner too
			else
				to_delete = true;

			return; 
		}

	}


	if (hasSpeed == true)
		SetPos(rect.x + speed.x, rect.y + speed.y);
}