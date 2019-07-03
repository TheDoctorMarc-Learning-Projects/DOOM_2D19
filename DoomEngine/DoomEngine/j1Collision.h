#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#include "j1Module.h"
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
	COLLIDER_LOOT,
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
	bool hasCallback = false; 
	bool hasSpeed = false; 
	fPoint speed = fPoint(0, 0); 

	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Entity* callback = nullptr, fPoint speed = fPoint(0,0)) :
		rect(rectangle),
		type(type),
		callback(callback),
		speed(speed)
	{
		if (!speed.IsZero())
			hasSpeed = true; 
	}

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
	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback = nullptr, fPoint speed = fPoint(0,0));
	void DebugDraw();
	
	
	void doCollisionAssignment(Collider* c1, Collider* c2);
	void doCollisionDeAssignment(Collider* c1, Collider* c2);


private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
};

#endif // __j1COLLISION_H__  