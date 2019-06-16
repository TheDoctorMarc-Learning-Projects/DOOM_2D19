#ifndef __j1PARTICLESCLASSIC_H__
#define __j1PARTICLESCLASSIC_H__

#include "j1Module.h"
#include "p2Animation.h"
#include "p2Log.h"
#include "p2Point.h"
#include <list>
//#include "j1Collision.h"

#define MAX_ACTIVE_PARTICLES 500

struct SDL_Texture;
//struct Collider;
enum COLLIDER_TYPE;

struct Particle // only dumb visual class
{
	//Collider* collider = nullptr;
	SDL_RendererFlip renderFlip = SDL_RendererFlip::SDL_FLIP_NONE;
	double angle = 0; 
	float scale = 1.F;
	iPoint pivot = { (int)INT_MAX, (int)INT_MAX };
	Animation anim;
	SDL_Texture* texture = nullptr;
	uint fx = 0u;
	iPoint position;
	//iPoint impactPosition = { NULL,NULL }; // TODO: to instantiate another particle on impact pos
	//bool impactPositioning = false;
	iPoint speed;
	Uint32 born = 0;
	Uint32 life = 0;
	//Uint32 damage = 0;
	bool fx_played = false;
	float parallaxSpeed = 1.0;
	bool useCameraScale = true; 
	
	bool onScreen = false; 

	Particle();
	Particle(const Particle& p);
	~Particle();
	bool Update(float dt);
	// variables to instantiate collision particle
	/*Particle* onCollisionWallParticle = nullptr; // TODO: 
	Particle* onCollisionGeneralParticle = nullptr;
	Particle* deathParticle = nullptr;*/

};

class j1ParticlesClassic : public j1Module
{
public:

	j1ParticlesClassic();
	~j1ParticlesClassic();

	bool Awake(pugi::xml_node& node);
	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();
	//void OnCollisionSubtile(Collider* c1, Collider* c2);

	//void AddParticle(const Particle& particle, Animation& sourceAnim, int x, int y, Uint32 delay = 0, iPoint speed = { 0,0 }, Uint32 life = 0, char* fx = nullptr);
	void AddParticle(const Particle& particle, int x, int y, iPoint speed = { 0,0 }, Uint32 delay = 0, SDL_RendererFlip rFlip = SDL_RendererFlip::SDL_FLIP_NONE, double angle = 0, int pivotx = INT_MAX, int pivoty = INT_MAX, float scale = 1.0F, float parallaxSpeed = 1.0F, bool useCameraScale = true, bool onSceen = false);

	//bool LoadAnimation(pugi::xml_node &node, Animation &anim, bool sequential = false);

private:

	SDL_Texture* particleAtlas2 = nullptr; 
	
	std::list<Particle*> active;
	std::list<Particle*> activeOnScreen;
	pugi::xml_node particleNode;

public:
	
	// TODO add particles:
};

#endif // __j1PARTICLES_H__