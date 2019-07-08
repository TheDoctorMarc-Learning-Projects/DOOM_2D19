#ifndef _J1BLOODDROP_H__
#define _J1BLOODDROP_H__

#include "j1Entity.h"
#include "j1Collision.h"
#include "j1EntityPlatformDynamic.h"
#include "Color.h"


class j1Entity;

struct Viscosity
{
	float speed = 3.f; 
	uint frameRate = 30U; 
	uint frameCounter = 0U; 
};

class j1EntityBloodDrop : public j1Entity
{
public:
	j1EntityBloodDrop(int posX, int posY, fPoint speed, Color c);
	~j1EntityBloodDrop();

	void Draw() override; // special draw, call bloodmanager draw and drawquad; 
	bool Update(float dt); 
	bool CleanUp() override; 

	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;

public: 
       // new collider in collision cpp, than can collide with floor, but ignores entitites for the mom
	Color c;
	fPoint initialSpeed = fPoint(0, 0); 
	bool floorReached = false; 
	bool roofReached = false; 
	SDL_Rect lastPosCollider = { 0,0,0,0 }; 
	Viscosity viscosityData; 
	j1EntityPlatformDynamic* dynGroundCallback = nullptr; 

	friend class j1BloodManager;


};

#endif 
