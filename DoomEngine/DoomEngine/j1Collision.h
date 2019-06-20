#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#include "j1Module.h"
#include "j1Entity.h"
#include "p2Point.h"
#include <list>


#define MAX_COLLIDERS 200

enum CollisionType
{
	TOP,
	BOTTOM,
	LATERAL
};

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_WALL,
	COLLIDER_FLOOR,
	COLLIDER_PLAYER, 
	COLLIDER_DEATH,
	COLLIDER_GOD,
	COLLIDER_WIN,
	COLLIDER_ENEMY,
	COLLIDER_SHOT,
	COLLIDER_MAX
};

struct SDL_Rect; 
class j1Entity; 

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Entity* callback = nullptr;

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Entity* callback = nullptr) :
		rect(rectangle),
		type(type),
		callback(callback)
	{}

	void SetPos(int x, int y)
	{
		rect.x = x;
		rect.y = y;
	}

	std::list<Collider*> onCollisionWithMe; 

	void AdaptCollider(int w, int h, int x = NULL, int y = NULL) {
		rect.w = w; 
		rect.h = h; 
		if (x)
			rect.x = x; 
		if (y)
			rect.y = y; 
	}

	CollisionType currentCollisionType; 

	bool CheckCollision(const SDL_Rect& r);
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate(); 
	bool Update(float dt);
	bool CleanUp(); 
	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback = nullptr);
	void DebugDraw();
	
	
	void doCollisionAssignment(Collider* c1, Collider* c2);
	void doCollisionDeAssignment(Collider* c1, Collider* c2);


private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
};

#endif // __j1COLLISION_H__  