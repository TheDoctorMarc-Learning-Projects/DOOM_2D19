#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "p2Log.h"
#include "Brofiler/Brofiler.h"
#include "Color.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;
	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WALL][COLLIDER_GOD] = true;
	matrix[COLLIDER_WALL][COLLIDER_WIN] = false;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_SHOT] = true;

	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SHOT] = true;

	matrix[COLLIDER_DEATH][COLLIDER_DEATH] = false;
	matrix[COLLIDER_DEATH][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WALL] = false;
	matrix[COLLIDER_DEATH][COLLIDER_GOD] = true;
	matrix[COLLIDER_DEATH][COLLIDER_WIN] = false;
	matrix[COLLIDER_DEATH][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_DEATH][COLLIDER_SHOT] = false;

	matrix[COLLIDER_GOD][COLLIDER_GOD] = false;
	matrix[COLLIDER_GOD][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_GOD][COLLIDER_WALL] = true;
	matrix[COLLIDER_GOD][COLLIDER_DEATH] = true;
	matrix[COLLIDER_GOD][COLLIDER_WIN] = true;
	matrix[COLLIDER_GOD][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_GOD][COLLIDER_SHOT] = true;

	matrix[COLLIDER_WIN][COLLIDER_WIN] = false;
	matrix[COLLIDER_WIN][COLLIDER_GOD] = true;
	matrix[COLLIDER_WIN][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WIN][COLLIDER_WALL] = false;
	matrix[COLLIDER_WIN][COLLIDER_DEATH] = false;
	matrix[COLLIDER_WIN][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_WIN][COLLIDER_SHOT] = false;

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WIN] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_GOD] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_SHOT] = true;


	matrix[COLLIDER_SHOT][COLLIDER_SHOT] = false;
	matrix[COLLIDER_SHOT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WIN] = false;
	matrix[COLLIDER_SHOT][COLLIDER_DEATH] = false;
	matrix[COLLIDER_SHOT][COLLIDER_GOD] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WALL] = true;
	matrix[COLLIDER_SHOT][COLLIDER_ENEMY] = true;
	
}

// Destructor
j1Collision::~j1Collision()
{}

bool j1Collision::PreUpdate()
{
	BROFILER_CATEGORY("Collision PreUpdate", Profiler::Color::DarkSeaGreen);
	bool ret = true; 
	// Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr && colliders[i]->to_delete == true)
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	Collider* c1;
	Collider* c2;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		// skip empty colliders
		if (colliders[i] == nullptr)
			continue;

		c1 = colliders[i];
		
		// avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k)
		{
			// skip empty colliders
			if (colliders[k] == nullptr)
				continue;

			c2 = colliders[k];

			if (c1->CheckCollision(c2->rect) == true)
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
			else      // useful when colliders "exit" a collision
			{
				if (matrix[c1->type][c2->type] && c1->callback)
					doCollisionDeAssignment(c1, c2);

				if (matrix[c2->type][c1->type] && c2->callback)
					doCollisionDeAssignment(c2, c1);
		
			}
		}
	}

	// Calculate collisions
	

	return ret;

}

void j1Collision::doCollisionAssignment(Collider* c1, Collider* c2)
{
	if (!c1->onCollisionWithMe.empty())
	{
		bool assigned = false;

		std::list<Collider*>::iterator onCollisionCol = c1->onCollisionWithMe.begin();
		for (; onCollisionCol != c1->onCollisionWithMe.end(); ++onCollisionCol)
		{
			if ((*onCollisionCol) == c2)
			{
				assigned = true;
				break; 
			}

		}

		if (!assigned)
			c1->onCollisionWithMe.push_back(c2);
	}
	else
	{
		c1->onCollisionWithMe.push_back(c2);
	}
	
}

void j1Collision::doCollisionDeAssignment(Collider* c1, Collider* c2)
{
	if (!c1->onCollisionWithMe.empty())
	{
		std::list<Collider*>::iterator onCollisionCol = c1->onCollisionWithMe.begin();
		for (; onCollisionCol != c1->onCollisionWithMe.end();)
		{
			if ((*onCollisionCol) == c2)
			{
				onCollisionCol = c1->onCollisionWithMe.erase(onCollisionCol);
				c1->callback->OnCollisionExit(c1, c2);
			}

			else
				++onCollisionCol;

		}

	}

}


// Called before render is available
bool j1Collision::Update(float dt)
{
	BROFILER_CATEGORY("Collision Update", Profiler::Color::DarkSlateBlue);
	bool ret = true;
	
	if (App->input->GetKey(SDL_SCANCODE_C) == KEY_DOWN)
		debug = !debug;

	if (debug)
		DebugDraw();

	return ret;
}

void j1Collision::DebugDraw()
{
	BROFILER_CATEGORY("Collision DebugDraw", Profiler::Color::DarkSlateGray);
	/*if (App->input->keyboard[SDL_SCANCODE_1] == KEY_DOWN) {
		if (debug == false) debug = true;
		else debug = false;
	}

	if (debug == false)
		return;*/
	
	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{

		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_WALL: // red
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_DEATH: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 0, alpha);
			break;
		case COLLIDER_GOD: // violet
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
			break;
		case COLLIDER_WIN: 
			App->render->DrawQuad(colliders[i]->rect, 255, 150, 255, alpha);
			break;
		case COLLIDER_ENEMY:
			App->render->DrawQuad(colliders[i]->rect, 255, 50, 255, alpha);
			break;
		case COLLIDER_SHOT:
			App->render->DrawQuad(colliders[i]->rect, 200, 150, 75, alpha);
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

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
		{
			delete colliders[i];
			
			colliders[i] = nullptr;
		}
	}

	return ret; 
}
                                                 

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback)
{
	BROFILER_CATEGORY("Collision AddCollider", Profiler::Color::DeepPink);
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback);
			break;
		}
	}

	return ret;
}

void j1Collision::AdaptCollider(Collider& col, SDL_Rect target)   // useful to prevent fake colliders depending on current entity anim
{
	col.rect = target; 
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	BROFILER_CATEGORY("Collision CheckCollision", Profiler::Color::DeepSkyBlue);
	bool ret = true;

	if (r.x + r.w < rect.x) ret = false;
	else if (r.x > rect.x + rect.w) ret = false;
	else if (r.y + r.h < rect.y)ret = false;
	else if (r.y > rect.h + rect.y)ret = false;

	return ret;
}