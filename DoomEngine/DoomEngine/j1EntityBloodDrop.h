#ifndef _J1BLOODDROP_H__
#define _J1BLOODDROP_H__

#include "j1Entity.h"
#include "j1Collision.h"
#include "j1EntityPlatformDynamic.h"
#include "Color.h"


class j1Entity;

struct Viscosity
{
	float speed = 4.f; 
	uint frameRate = 40U; 
	uint frameCounter = 0U; 
};

struct sideDynFloorType
{
	bool left = false; 
	bool right = false; 
	float sideOffset = 0.f; 
};

class j1EntityBloodDrop : public j1Entity
{
public:
	j1EntityBloodDrop(int posX, int posY, fPoint speed, Color c);
	~j1EntityBloodDrop();

	void Draw() override; // special draw, call bloodmanager draw and drawquad; 
	bool Update(float dt); 
	bool CleanUp() override; 

	virtual bool Load(pugi::xml_node&);
	virtual bool Save(pugi::xml_node&) const;



	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;

private: 
       // new collider in collision cpp, than can collide with floor, but ignores entitites for the mom
	Color c;
	fPoint initialSpeed = fPoint(0, 0); 
	bool floorReached = false; 
	bool roofReached = false; 
	bool collidedWithVerticalWall = false;
	sideDynFloorType sideDynPlatfColType;
	SDL_Rect lastPosCollider = { 0,0,0,0 }; 
	Viscosity viscosityData; 
	j1EntityPlatformDynamic* dynGroundCallback = nullptr; 
	j1EntityPlatformDynamic* lastDynPlatf = nullptr; 

	friend class j1BloodManager;


	bool otherBloodDropInFloor = false; 


};

#endif 
