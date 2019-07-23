#ifndef __j1Particles_H__
#define __j1Particles_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Log.h"
#include "p2Point.h"
#include <list>
#include <map>
#include <string>
#include "j1Collision.h"
#include "j1Entity.h"
#define MAX_ACTIVE_PARTICLES 500

struct SDL_Texture;
struct Collider;
enum COLLIDER_TYPE;

struct Particle // only dumb visual class
{
	std::string name; 
	Collider* collider = nullptr;
	bool hasCollider = false; 
	SDL_RendererFlip renderFlip = SDL_RendererFlip::SDL_FLIP_NONE;
	double angle = 0; 
	float scale = 1.F;
	iPoint pivot = { (int)INT_MAX, (int)INT_MAX };
	Animation anim;
	std::string fx = ""; 
	fPoint position;
	fPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	//Uint32 damage = 0;
	bool fx_played = false;
	float parallaxSpeed = 1.0;
	bool useCameraScale = true; 
	bool onScreen = false; 
	bool to_delete = false; 

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);


};

class j1Particles : public j1Module
{
public:

	j1Particles();
	~j1Particles();

	bool Awake(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	void AddParticle(std::string nameAtMap, int x, int y, j1Entity* callback = nullptr, bool assignToCollider = false, COLLIDER_TYPE colType = COLLIDER_NONE, fPoint speed = { 0,0 }, Uint32 delay = 0, SDL_RendererFlip rFlip = SDL_RendererFlip::SDL_FLIP_NONE, double angle = 0, int pivotx = INT_MAX, int pivoty = INT_MAX, float scale = 1.0F, float parallaxSpeed = 1.0F, bool useCameraScale = true, bool onSceen = false);
	Particle* AddParticleRet(std::string nameAtMap, int x, int y, j1Entity* callback = nullptr, bool assignToCollider = false, COLLIDER_TYPE colType = COLLIDER_NONE, fPoint speed = { 0,0 }, Uint32 delay = 0, SDL_RendererFlip rFlip = SDL_RendererFlip::SDL_FLIP_NONE, double angle = 0, int pivotx = INT_MAX, int pivoty = INT_MAX, float scale = 1.0F, float parallaxSpeed = 1.0F, bool useCameraScale = true, bool onSceen = false);

private:

	SDL_Texture* texture = nullptr; 
	
	
	pugi::xml_node particleNode;


	std::map<std::string, Particle> particleMap;




public:
	
	std::list<Particle*> active;
};

#endif // __j1Particles_H__