#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collision.h"
#include "p2Log.h"
#include "Brofiler/Brofiler.h"
#include "Color.h"
#include "j1Entity.h"

j1Collision::j1Collision()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

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


	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEATH] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_GOD] = false;
	matrix[COLLIDER_PLAYER][COLLIDER_WIN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_SHOT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_LOOT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_BLOOD] = false;

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

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WIN] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_DEATH] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_GOD] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_SHOT] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_LOOT] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_BLOOD] = false;

	matrix[COLLIDER_SHOT][COLLIDER_SHOT] = false;
	matrix[COLLIDER_SHOT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WIN] = false;
	matrix[COLLIDER_SHOT][COLLIDER_DEATH] = false;
	matrix[COLLIDER_SHOT][COLLIDER_GOD] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WALL] = true;
	matrix[COLLIDER_SHOT][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_SHOT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_SHOT][COLLIDER_LOOT] = false; 
	matrix[COLLIDER_SHOT][COLLIDER_BLOOD] = false;
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

			if (!colliders[i]->onCollisionWithMe.empty())
			{
				for (auto col = colliders[i]->onCollisionWithMe.begin(); col != colliders[i]->onCollisionWithMe.end();)    // put both pointers to each other to null when a col is deleted
				{
					bool deleteIt = false; 

					if (!(*col)->onCollisionWithMe.empty())
					{
						for (auto colReverse = (*col)->onCollisionWithMe.begin(); colReverse != (*col)->onCollisionWithMe.end();)
						{
							if ((*colReverse) == colliders[i])  // is I find my self in another collider on collision with me, delete me from the list, and also delete it from my list
							{
								colReverse = (*col)->onCollisionWithMe.erase(colReverse);  // delete myself from ther other's lits
								deleteIt = true;
							}
							else
							{
								++colReverse;
							}
						}
					}
					

					if (deleteIt)
					{
						col = colliders[i]->onCollisionWithMe.erase(col);  // delete the other from my list

					}
					else
					{
						++col;
					}
				}
					
			}
			delete colliders[i];
			colliders[i] = nullptr;
		}
		else if(colliders[i] != nullptr)
		{
			if (App->render->IsOnCamera(colliders[i]->rect.x, colliders[i]->rect.y, colliders[i]->rect.w, colliders[i]->rect.h))
			{
				if (colliders[i]->hasSpeed)
					colliders[i]->SetPos(colliders[i]->rect.x + colliders[i]->speed.x, colliders[i]->rect.y + colliders[i]->speed.y);
			}
			else
			{
				if (colliders[i]->volatileOutOfScreen)  // delete whe out of camera limits 
					colliders[i]->to_delete = true; 
			}


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
		{
			c1->onCollisionWithMe.push_back(c2);

		}
		
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
			App->render->DrawQuad(colliders[i]->rect, 200, 250, 200, alpha);
			break;
		case COLLIDER_FLOOR:
			App->render->DrawQuad(colliders[i]->rect, 50, 50, 255, alpha);
			break;
		case COLLIDER_LOOT:
			App->render->DrawQuad(colliders[i]->rect, 150, 230, 200, alpha);
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
			if (!colliders[i]->onCollisionWithMe.empty())
			{
				for (auto col = colliders[i]->onCollisionWithMe.begin(); col != colliders[i]->onCollisionWithMe.end();)    // put both pointers to each other to null when a col is deleted
				{
					bool deleteIt = false;

					if (!(*col)->onCollisionWithMe.empty())
					{
						for (auto colReverse = (*col)->onCollisionWithMe.begin(); colReverse != (*col)->onCollisionWithMe.end();)
						{
							if ((*colReverse) == colliders[i])  // is I find my self in another collider on collision with me, delete me from the list, and also delete it from my list
							{
								colReverse = (*col)->onCollisionWithMe.erase(colReverse);  // delete myself from ther other's lits
								deleteIt = true;
							}
							else
							{
								++colReverse;
							}
						}
					}


					if (deleteIt)
					{
						col = colliders[i]->onCollisionWithMe.erase(col);  // delete the other from my list

					}
					else
					{
						++col;
					}
				}

			}
			delete colliders[i];

			colliders[i] = nullptr;
		}
		else
			break;    // one you encounter a nullptr pos, do not proceed until MAX_COLLIDERS
	}

	return ret; 
}
                                                 

Collider* j1Collision::AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback, fPoint speed, bool volatileOutOfScreen)
{
	BROFILER_CATEGORY("Collision AddCollider", Profiler::Color::DeepPink);
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, type, callback, speed, volatileOutOfScreen);

			if (callback)
				ret->hasCallback = true; 

			break;
		}
	}

	return ret;
}


// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) 
{
	BROFILER_CATEGORY("Collision CheckCollision", Profiler::Color::DeepSkyBlue);
	/*bool ret = false;

	if (r.x + r.w > rect.x && r.x < rect.x + rect.w && r.y + r.h > rect.y && r.y < rect.h + rect.y)
		ret = true;

	return ret;*/

	bool ret = true;

	if (r.x + r.w < rect.x) ret = false;
	else if (r.x > rect.x + rect.w) ret = false;
	else if (r.y + r.h < rect.y)ret = false;
	else if (r.y > rect.h + rect.y)ret = false;

	return ret;
}

