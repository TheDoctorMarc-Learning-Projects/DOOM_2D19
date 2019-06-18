#ifndef __J1ENTITY_H__
#define __J1ENTITY_H__

#include "p2Animation.h"
#include "p2Point.h"
#include "j1Timer.h"
#include "j1PerfTimer.h"
#include "j1Module.h"
#include "j1Textures.h"


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
	//virtual void OnCollision(Collider* collider);

//	iPoint GetTilePos() const;
	fPoint GetPosition();
//	bool ChangedTile() const; 

	//virtual void LoadEntitydata(pugi::xml_node&);

	

public:
	SDL_Rect atlasRect;
	bool					to_delete = false;
	bool					to_die = false; 
	std::string				name = "\0";
	fPoint					position;
	iPoint					size;
	ENTITY_TYPE				type;
	SDL_RendererFlip		flip = SDL_FLIP_NONE;

	float					life = 100.f;
	float					maxLife = 100.f;
	float					defence = 0.f;
	pugi::xml_document		file;
	pugi::xml_parse_result	result;

	bool					isInRange = false;
	bool					isParalize = false;

	Collider* collider = nullptr;
	Animation				idle; 
	Animation*				currentAnimation = nullptr;
	SDL_Texture*			entityTex = nullptr;


protected:
	iPoint imOnTile;
	iPoint previousTilePos; 
};

#endif
