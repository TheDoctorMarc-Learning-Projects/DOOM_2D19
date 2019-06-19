#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Animation.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "j1PerfTimer.h"
#include "j1Module.h"
#include "j1Textures.h"
#include "j1Collision.h"

#include "SDL_image/include/SDL_image.h"

#include <math.h>



enum ENTITY_TYPE  // todo, pass to class
{
		NO_TYPE,
		ENTITY_STATIC,
		ENTITY_DYNAMIC,
		PLAYER,
		MAX_TYPE
};

enum POINTING_DIR
{
	RIGHT,
	LEFT
};


class Collider; 

struct EntityInfo
{
	iPoint position;
	//Collider* collider = nullptr;
};

class j1Entity
{
public:
	j1Entity(ENTITY_TYPE type, float positionX, float positionY, std::string name);
	j1Entity() {};



	virtual ~j1Entity();

	virtual bool Start();
	
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();
	virtual bool CleanUp();


	virtual bool Move(float dt);
	virtual void Draw();
	virtual void OnCollision(Collider* c1, Collider* c2) {};

	virtual void OnCollisionExit(Collider* c1, Collider* c2) {};
//	iPoint GetTilePos() const;
	fPoint GetPosition();
//	bool ChangedTile() const; 
	POINTING_DIR GetDirection(); 
	//virtual void LoadEntitydata(pugi::xml_node&);

	void ResetGravity(); 

	void AdjustColliderToAnimFrame(); 

public:
	SDL_Rect atlasRect;
	bool					to_delete = false;
	bool					to_die = false; 
	bool                    onPlatform = false; 
	std::string				name = "\0";
	fPoint					position;
	fPoint                  previousPosition; 
	iPoint					size;
	float                   spriteScale = 1.f; 
	ENTITY_TYPE				type;
	SDL_RendererFlip		flip = SDL_FLIP_NONE;
	POINTING_DIR            pointingDir = POINTING_DIR::LEFT; 

	float					life = (float)LONG_MAX;
	float					maxLife = (float)LONG_MAX;
	float					speed = (float)LONG_MAX;
	float                   gravityFactor = (float)LONG_MAX; 
	float                   mass = (float)LONG_MAX; 
	Collider*               collider = nullptr;
	Animation				idle; 
	Animation*				currentAnimation = nullptr;
	SDL_Texture*			entityTex = nullptr;            // specific entity texture


protected:
	iPoint imOnTile;
	iPoint previousTilePos; 
};

#endif
