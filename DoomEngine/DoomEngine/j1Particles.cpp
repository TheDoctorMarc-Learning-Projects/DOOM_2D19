#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "Brofiler/Brofiler.h"
#include "j1Window.h"
//#include "SDL/include/SDL_timer.h"

#define MARGIN 20

j1Particles::j1Particles()
{
	name.assign("particles");
}

j1Particles::~j1Particles()
{}

bool j1Particles::Awake(pugi::xml_node& node)
{
	LOG("AWAKING PARTICLES");
	if (node.empty())
		LOG("empty particles node");

	particleNode = node;

	return true;
}

// Load assets
bool j1Particles::Start()
{
	LOG("Loading particles");


	texture = App->tex->Load("textures/particles/particles.png"); 

	// - - - - - - - - - - - - - - - - - - - - - - - WEAPONS
	// shotgun shot fire 
	Particle shotgunShotFire;
	shotgunShotFire.name = "shotgunShotFire";
	shotgunShotFire.anim.PushBack({0, 298, 7, 6});
	shotgunShotFire.life = 75;
	shotgunShotFire.speed.create(0, 0);
	shotgunShotFire.anim.speed = 0.5;
	shotgunShotFire.anim.loop = false;
	shotgunShotFire.fx = "shotgunShotFire"; 

	particleMap.insert(std::make_pair(shotgunShotFire.name, shotgunShotFire));


	// machine gun shot fire 
	Particle machineGunShotFire;
	machineGunShotFire.name = "machineGunShotFire";
	machineGunShotFire.anim.PushBack({ 0, 307, 7, 6 });
	machineGunShotFire.life = 20;
	machineGunShotFire.speed.create(0, 0);
	machineGunShotFire.anim.speed = 0.5;
	machineGunShotFire.anim.loop = false;
	machineGunShotFire.fx = "machineGunShotFire";

	particleMap.insert(std::make_pair(machineGunShotFire.name, machineGunShotFire));

	// - - - - - - - - - - - - - - - - - - - - - - - ENEMIES
	// Cacodemon
	Particle EnemyCacodemonShot;                                   // the other data is defined in enemy shoot function 
	EnemyCacodemonShot.name = "EnemyCacodemonShot";
	EnemyCacodemonShot.anim.PushBack({ 1, 129, 23, 23 });  
	EnemyCacodemonShot.anim.PushBack({ 25, 129, 23, 23 });
	EnemyCacodemonShot.anim.speed = 5.3;
	EnemyCacodemonShot.anim.loop = true; 

	particleMap.insert(std::make_pair(EnemyCacodemonShot.name, EnemyCacodemonShot));

	Particle EnemyCacodemonShotExplosion;                                   // the other data is defined in enemy shoot function 
	EnemyCacodemonShotExplosion.name = "EnemyCacodemonShotExplosion";
	EnemyCacodemonShotExplosion.anim.PushBack({ 49, 112, 41, 40 });
	EnemyCacodemonShotExplosion.anim.PushBack({ 91, 109, 50, 43 });
	EnemyCacodemonShotExplosion.anim.PushBack({ 142, 106, 53, 46 });
	EnemyCacodemonShotExplosion.speed.create(0, 0);
	EnemyCacodemonShotExplosion.anim.speed = 12.6;
	EnemyCacodemonShotExplosion.anim.loop = false;

	particleMap.insert(std::make_pair(EnemyCacodemonShotExplosion.name, EnemyCacodemonShotExplosion));



	
	// Baron Of Hell
	Particle EnemyBaronOfHellShot;                                   // the other data is defined in enemy shoot function 
	EnemyBaronOfHellShot.name = "EnemyBaronOfHellShot";
	EnemyBaronOfHellShot.anim.PushBack({ 5, 225, 35, 10 });
	EnemyBaronOfHellShot.anim.PushBack({ 3, 242, 33, 10 });
	EnemyBaronOfHellShot.anim.PushBack({ 41, 223, 48, 12 });
	EnemyBaronOfHellShot.anim.PushBack({ 37, 240, 49, 12 });
	EnemyBaronOfHellShot.anim.PushBack({ 90, 222, 33, 13 });
	EnemyBaronOfHellShot.anim.PushBack({ 87, 239, 35, 13 });
	EnemyBaronOfHellShot.life = (uint32)(uint)INT_MAX;      // if the anim does not have loop, keep it alive unless out of camera
	EnemyBaronOfHellShot.anim.speed = 5.3;
	EnemyBaronOfHellShot.anim.loop = false;

	particleMap.insert(std::make_pair(EnemyBaronOfHellShot.name, EnemyBaronOfHellShot));

	Particle EnemyBaronOfHellShotExplosion;                                   // the other data is defined in enemy shoot function 
	EnemyBaronOfHellShotExplosion.name = "EnemyBaronOfHellShotExplosion";
	EnemyBaronOfHellShotExplosion.anim.PushBack({ 1, 260, 33, 33 });
	EnemyBaronOfHellShotExplosion.anim.PushBack({ 35, 257, 41, 36 });
	EnemyBaronOfHellShotExplosion.anim.PushBack({ 77, 253, 45, 40 });
	EnemyBaronOfHellShotExplosion.speed.create(0, 0);
	EnemyBaronOfHellShotExplosion.anim.speed = 12.6;
	EnemyBaronOfHellShotExplosion.anim.loop = false;

	particleMap.insert(std::make_pair(EnemyBaronOfHellShotExplosion.name, EnemyBaronOfHellShotExplosion));
	

	

	
	return true;
}

// Unload assets
bool j1Particles::CleanUp()
{
	LOG("Unloading particles");

	//unloading graphics
	
	if (texture != nullptr)
	{
		App->tex->UnLoad(texture);    
		texture = nullptr;
	}

	RemoveActive(); 

	particleMap.clear(); // check if content is stil there

	return true;
}

bool j1Particles::Update(float dt)
{
	bool ret = true;


	return ret;
}


// Update: draw background
bool j1Particles::PostUpdate()//float dt)
{
	BROFILER_CATEGORY("Particles PostUpdate", Profiler::Color::BlanchedAlmond);

	for (std::list<Particle*>::iterator p = active.begin(); p != active.end();)
	{

		if ((*p)->Update(App->GetDt()) == false)
		{
			if ((*p)->hasCollider)
				(*p)->collider->to_delete = true; 

			delete (*p);
			(*p) = nullptr;
			p = active.erase(p);
		}
		else if (SDL_GetTicks() >= (*p)->born)            // TODO: update collider pos if hasCollider 
		{
			App->render->Blit(texture, (*p)->position.x, (*p)->position.y, &(*p)->anim.GetCurrentFrame(), (*p)->parallaxSpeed, (*p)->renderFlip, (*p)->scale, (*p)->angle, (*p)->pivot.x * App->win->GetScale(), (*p)->pivot.y * App->win->GetScale(), (*p)->useCameraScale);

			if ((*p)->hasCollider)
				(*p)->collider->SetPos((*p)->position.x, (*p)->position.y);   

			if ((*p)->fx_played == false && (*p)->fx != "")
			{
				(*p)->fx_played = true;
				// Play particle fx here
				App->audio->PlayFx((*p)->fx, 0);    
			}
			++p;
		}
		else
			++p;
	}

	return true;
}

//void ModuleParticles::AddParticle(const Particle& particle, Animation& sourceAnim, int x, int y, Uint32 delay, iPoint speed, Uint32 life, char* name)
void j1Particles::AddParticle(std::string nameAtMap, int x, int y, j1Entity* callback, bool assignToCollider, COLLIDER_TYPE coltype, fPoint speed, Uint32 delay, SDL_RendererFlip rFlip, double angle, int pivotx, int pivoty, float scale, float parallaxSpeed, bool useCameraScale, bool onScreen)
{
	Particle* p = DBG_NEW Particle(particleMap.at(nameAtMap));

	if (coltype != COLLIDER_TYPE::COLLIDER_NONE)
	{
		p->hasCollider = true; 
		p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), coltype, callback, { 0,0 }, true);   // TODO: like player, update the collider rect according to frame ??? or is it overkill ???
		p->collider->SetPos(p->position.x, p->position.y);


		if (assignToCollider)
			p->collider->owner = p;
	}
		
	
	p->born = SDL_GetTicks() + delay;
	p->position.x = x;
	p->position.y = y;
	if (speed.x != 0 || speed.y != 0)  
	{
		p->speed = speed;
	}
	p->renderFlip = rFlip;
	p->angle = angle;
	if (pivotx != INT_MAX && pivoty != INT_MAX)
	{
		p->pivot.x = pivotx;
		p->pivot.y = pivoty;
		p->position.x -= p->pivot.x; 
		p->position.y -= p->pivot.y; 
	}

	p->scale = scale; 
	p->parallaxSpeed = parallaxSpeed;
	p->useCameraScale = useCameraScale;
	p->onScreen = onScreen; 

	
	active.push_back(p);
	
}


Particle* j1Particles::AddParticleRet(std::string nameAtMap, int x, int y, j1Entity* callback, bool assignToCollider, COLLIDER_TYPE coltype, fPoint speed, Uint32 delay, SDL_RendererFlip rFlip, double angle, int pivotx, int pivoty, float scale, float parallaxSpeed, bool useCameraScale, bool onScreen)
{
	Particle* p = DBG_NEW Particle(particleMap.at(nameAtMap));

	if (coltype != COLLIDER_TYPE::COLLIDER_NONE)
	{
		p->hasCollider = true;
		p->collider = App->collision->AddCollider(p->anim.GetCurrentFrame(), coltype, callback, { 0,0 }, true);   // TODO: like player, update the collider rect according to frame ??? or is it overkill ???
		p->collider->SetPos(p->position.x, p->position.y);


		if (assignToCollider)
			p->collider->owner = p;
	}


	p->born = SDL_GetTicks() + delay;
	p->position.x = x;
	p->position.y = y;
	if (speed.x != 0 || speed.y != 0)
	{
		p->speed = speed; 
	}
	p->renderFlip = rFlip;
	p->angle = angle;
	if (pivotx != INT_MAX && pivoty != INT_MAX)
	{
		p->pivot.x = pivotx;
		p->pivot.y = pivoty;
		p->position.x -= p->pivot.x;
		p->position.y -= p->pivot.y;
	}

	p->scale = scale;
	p->parallaxSpeed = parallaxSpeed;
	p->useCameraScale = useCameraScale;
	p->onScreen = onScreen;


	active.push_back(p);


	return p; 

}


void j1Particles::RemoveActive()
{
	if (!active.empty())
	{
		for (auto& particle : active)
			RELEASE(particle);
		active.clear();
	}

}
/*
bool j1Particles::Load(pugi::xml_node& node)
{
 


}

bool j1Particles::Save(pugi::xml_node& node) const
{
	 
	for (auto& particle : active)
		particle->Save(node);

	return true;
}
*/

Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed), fx(p.fx), born(p.born), life(p.life), renderFlip(p.renderFlip)
	
{}

Particle::~Particle()
{
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}


	// destroy particle when animation is finished ----> only if you do not define max life 
	if (!anim.loop && anim.Finished() && life == 0)
		ret = false;

	if (to_delete)
		ret = false; 

	position.x += speed.x * dt;
	position.y += speed.y * dt;
 

	return ret;
}



