#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1ParticlesClassic.h"
#include "j1Audio.h"
#include "Brofiler/Brofiler.h"
#include "j1Window.h"
//#include "SDL/include/SDL_timer.h"

#define MARGIN 20

j1ParticlesClassic::j1ParticlesClassic()
{
	name.assign("ParticlesClassic");
}

j1ParticlesClassic::~j1ParticlesClassic()
{}

bool j1ParticlesClassic::Awake(pugi::xml_node& node)
{
	LOG("AWAKING PARTICLES");
	if (node.empty())
		LOG("empty particles node");

	particleNode = node;

	return true;
}

// Load assets
bool j1ParticlesClassic::Start()
{
	LOG("Loading particles");



	//particleAtlas2 = App->tex->Load("textures/particles/BuffParticles2.png");   // TODO: change atlas path, add  new articles to it
	
	return true;
}

// Unload assets
bool j1ParticlesClassic::CleanUp()
{
	LOG("Unloading particles");

	//unloading graphics
	
/*	if (particleAtlas2 != nullptr)
	{
		App->tex->UnLoad(particleAtlas2);    // TODO: change atlas path, add  new articles to it
		particleAtlas2 = nullptr;
	}*/


	//removing active particles
	if (!active.empty())
	{
		std::list<Particle*>::iterator particles = active.begin();

		for (; particles != active.end();)
		{
			if ((*particles)->texture != nullptr)
			{
				App->tex->UnLoad((*particles)->texture);
				(*particles)->texture = nullptr;
			}
			delete (*particles);
			(*particles) = nullptr;
			particles = active.erase(particles);
		}
		active.clear();
	}


	if (!activeOnScreen.empty())
	{
		std::list<Particle*>::iterator particles = activeOnScreen.begin();

		for (; particles != activeOnScreen.end();)
		{
			if ((*particles)->texture != nullptr)
			{
				App->tex->UnLoad((*particles)->texture);
				(*particles)->texture = nullptr;
			}
			delete (*particles);
			(*particles) = nullptr;
			particles = activeOnScreen.erase(particles);
		}
		activeOnScreen.clear();
	}
	//removing particles FX audio
	//App->audio->UnloadSFX();

	return true;
}

bool j1ParticlesClassic::Update(float dt)
{
	bool ret = true;


	return ret;
}


// Update: draw background
bool j1ParticlesClassic::PostUpdate()//float dt)
{
	BROFILER_CATEGORY("Particles PostUpdate", Profiler::Color::BlanchedAlmond);

	for (std::list<Particle*>::iterator p = active.begin(); p != active.end();)
	{

		if ((*p)->Update(App->GetDt()) == false)
		{
			delete (*p);
			(*p) = nullptr;
			p = active.erase(p);
		}
		else if (SDL_GetTicks() >= (*p)->born)
		{
			App->render->Blit((*p)->texture, (*p)->position.x, (*p)->position.y, &(*p)->anim.GetCurrentFrame(), (*p)->parallaxSpeed, (*p)->renderFlip, (*p)->scale, (*p)->angle, (*p)->pivot.x * App->win->GetScale(), (*p)->pivot.y * App->win->GetScale(), (*p)->useCameraScale);
			if ((*p)->fx_played == false && (*p)->fx != 0)
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
void j1ParticlesClassic::AddParticle(const Particle& particle, int x, int y, iPoint speed, Uint32 delay, SDL_RendererFlip rFlip, double angle, int pivotx, int pivoty, float scale, float parallaxSpeed, bool useCameraScale, bool onScreen)
{
	Particle* p = DBG_NEW Particle(particle);
	p->born = SDL_GetTicks() + delay;
	p->position.x = x;
	p->position.y = y;
	if (speed.x != 0 || speed.y != 0) //if we send specific speed, defines it
	{
		p->speed = speed;
	}
	p->renderFlip = rFlip;
	p->angle = angle;
	if (pivotx != INT_MAX && pivoty != INT_MAX)
	{
		p->pivot.x = pivotx;
		p->pivot.y = pivoty;
		p->position -= p->pivot;
	}

	p->scale = scale; 
	p->parallaxSpeed = parallaxSpeed;
	p->useCameraScale = useCameraScale;
	p->onScreen = onScreen; 

	if (!p->onScreen)
		active.push_back(p);
	else
		activeOnScreen.push_back(p);
}


Particle::Particle()
{
	position.SetToZero();
	speed.SetToZero();
}

Particle::Particle(const Particle& p) :
	anim(p.anim), position(p.position), speed(p.speed), fx(p.fx), born(p.born), life(p.life), texture(p.texture), renderFlip(p.renderFlip)/*,
	damage(p.damage), onCollisionGeneralParticle(p.onCollisionGeneralParticle), onCollisionWallParticle(p.onCollisionWallParticle),
	impactPosition(p.impactPosition), deathParticle(p.deathParticle)*/
{}

Particle::~Particle()
{
	/*if (collider != nullptr)
		App->collision->DeleteCollider(collider);*/
	//App->entityFactory->DeleteEntityFromSubtile(this);
}

bool Particle::Update(float dt)
{
	bool ret = true;

	if (life > 0)
	{
		if ((SDL_GetTicks() - born) > life)
			ret = false;
	}

	//destroy particles respect camera position margins // TODO PARTICLES
	/*if (position.x > (abs(App->render->camera.x) / SCREEN_SIZE) + SCREEN_WIDTH - MARGIN)
		ret = false;
	else if (position.x < (abs(App->render->camera.x) / SCREEN_SIZE) - MARGIN - 150)
		ret = false;*/

		// destroy particle when animation is finished
	if (!anim.loop && anim.Finished())
		ret = false;

	position.x += speed.x * dt;
	position.y += speed.y * dt;

	/*if (collider != nullptr)
		collider->SetPos(position.x, position.y);*/

	return ret;
}

