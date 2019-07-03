#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Animation.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "j1PerfTimer.h"
#include "j1Module.h"
#include "j1Textures.h"
#include "j1Collision.h"
#include "j1Render.h"
#include "j1Audio.h"
#include <array>
#include <math.h>



#include "SDL_image/include/SDL_image.h"

#include <math.h>

#define EXTRA_DAMAGE_TO_TRIGGER_BRUTAL_DEATH 40

#define DEFAULT_GRAV 50.f	
#define MAX_GRAV 550.f


inline float GravityCalc(float& gm, float& m)  // g  and mass inline calc container
{
	(gm < MAX_GRAV / m) ? gm *= 1.05f : gm = MAX_GRAV * m;

	return gm;
}

enum ENTITY_TYPE  // todo, pass to class
{
		NO_TYPE,
		ENTITY_STATIC,
		ENTITY_DYNAMIC,
		PLAYER,
		MAX_TYPE,
		ENEMY_STATIC,
		ENEMY_IMP,
		LOOT,
};

enum POINTING_DIR
{
	RIGHT,
	LEFT,
	UP,    // for dyn platforms
	DOWN   // for dyn platforms   
};

class Collider; 

class j1Entity
{
public:
	j1Entity(ENTITY_TYPE type, float positionX, float positionY, std::string name);
	j1Entity(float positionX, float positionY);
	j1Entity(ENTITY_TYPE type, std::string name);
	j1Entity() {};



	virtual ~j1Entity();

	virtual bool Start();
	
	virtual bool PreUpdate();
	virtual bool Update(float dt);
	virtual bool PostUpdate();
	virtual bool CleanUp();
	virtual void SetDeathAnim(bool brutal = false) {};
	virtual void CheckDeathFinished() {};

	virtual bool Move(float dt);
	virtual void Draw();
	virtual void OnCollision(Collider* c1, Collider* c2) {};

	virtual void OnCollisionExit(Collider* c1, Collider* c2) {};
//	iPoint GetTilePos() const;
	fPoint GetPosition();
//	bool ChangedTile() const; 
	virtual POINTING_DIR GetDirection(); 
	//virtual void LoadEntitydata(pugi::xml_node&);

	void ResetGravity(); 


public:
	SDL_Rect                section = { 0, 0, 0, 0 }; 
	bool					to_delete = false;
	bool					to_die = false; 
	bool                    drawActive = true; 
	bool                    onPlatform = false; 
	bool                    useRenderFlip = false; 
	std::string				name = "no name";
	fPoint					position = fPoint(0, 0);
	fPoint                  previousPosition = fPoint(0, 0);
	iPoint					size = iPoint(0,0);
	float                   spriteScale = 1.f; 
	ENTITY_TYPE				type = ENTITY_TYPE::NO_TYPE;
	SDL_RendererFlip		flip = SDL_FLIP_NONE;
	POINTING_DIR            pointingDir = POINTING_DIR::LEFT; 
	POINTING_DIR            defaultPointingDir = POINTING_DIR::LEFT; 
	fPoint                  lastSpeed = fPoint(0, 0);
	int				     	life = INT_MAX;
	int					    maxLife = INT_MAX;
	float					speed = (float)LONG_MAX;
	float                   gravityFactor = (float)LONG_MAX; 
	float                   mass = (float)LONG_MAX; 
	Collider*               collider = nullptr;
	Animation				idle; 
	Animation*				currentAnimation = nullptr;
	SDL_Texture*			entityTex = nullptr;            // specific entity texture
	double                  spriteRotation = 0; 
	iPoint                  rotationPivots = iPoint(INT_MAX, INT_MAX); 
	//uint                    blitOrder = 0; 
};

#endif
