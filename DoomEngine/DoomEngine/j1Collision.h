#ifndef __j1COLLISION_H__
#define __j1COLLISION_H__

#include "j1Module.h"
#include "p2Point.h"
#include <list>
#include <vector>
#include <memory>

#include "SDL/include/SDL_rect.h"

enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_PRESENTIAL,
	COLLIDER_WALL,
	COLLIDER_FLOOR,
	COLLIDER_PLAYER, 
	COLLIDER_GOD,
	COLLIDER_WIN,
	COLLIDER_ENEMY,
	COLLIDER_SHOT,
	COLLIDER_ENEMY_SHOT,
	COLLIDER_LOOT,
	COLLIDER_BLOOD,
	COLLIDER_WALL_DETECTION,
	COLLIDER_MAX
};

struct SDL_Rect; 
class j1Entity; 

struct Particle; 

struct Collider
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Entity* callback = nullptr;
	Particle* owner = nullptr; // eg shot creates collider, but callback is instead the enemy
	bool hasCallback = false; 
	bool hasSpeed = false; 
	bool volatileOutOfScreen = false; 
	fPoint speed = fPoint(0, 0); 
	fPoint initialPos = fPoint(0, 0);  // needed sometimes like shot, to compare first and last pos

	Collider() {}
	Collider(SDL_Rect rectangle, COLLIDER_TYPE type, j1Entity* callback = nullptr, fPoint speed = fPoint(0,0), bool volatileOutOfScreen = false) :
		rect(rectangle),
		type(type),
		callback(callback),
		speed(speed),
		volatileOutOfScreen(volatileOutOfScreen)
	{
		if (speed.IsZero() == false)
		{
			hasSpeed = true;
			initialPos = fPoint((float)rect.x, (float)rect.y); 
		}
			
		if (callback != nullptr)
			hasCallback = true; 

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

	bool CheckCollision(const SDL_Rect& r);

	void FreeOnCollision(); 
	void UpdateStatus(); 
};

class j1Collision : public j1Module
{
public:

	j1Collision();
	~j1Collision();

	bool PreUpdate(); 
	bool Update(float dt);
	bool CleanUp(); 

	bool Load(pugi::xml_node& node);            // Save & Load will be used with collider player shots 
	bool Save(pugi::xml_node& node) const;


	Collider* AddCollider(SDL_Rect rect, COLLIDER_TYPE type, j1Entity* callback = nullptr, fPoint speed = fPoint(0,0), bool volatileOutOfScreen = false);
	void DebugDraw();
	void DestroyCollider(Collider* col); 
	
	void doCollisionAssignment(Collider* c1, Collider* c2);
	void doCollisionDeAssignment(Collider* c1, Collider* c2);


private:
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
public: 
	bool debug = false;
	std::vector<Collider*> colliders; 
	
};

#endif // __j1COLLISION_H__  