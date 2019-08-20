#include <math.h>
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Particles.h"
#include "j1Audio.h"
#include "j1EntityFactory.h"
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

	p->name = nameAtMap;

	if (coltype != COLLIDER_TYPE::COLLIDER_NONE)
	{
		p->hasCollider = true; 
		SDL_Rect targetRect = { x, y, p->anim.GetCurrentFrame().w, p->anim.GetCurrentFrame().h }; 
		p->collider = App->collision->AddCollider(targetRect, coltype, callback, { 0,0 }, true);    
	
		if (assignToCollider)
		{
			p->IOwnTheCollider = true;
			p->collider->owner = p;
		}
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

	p->name = nameAtMap; 

	if (coltype != COLLIDER_TYPE::COLLIDER_NONE)
	{
		p->hasCollider = true;
		SDL_Rect targetRect = { x, y, p->anim.GetCurrentFrame().w, p->anim.GetCurrentFrame().h };
		p->collider = App->collision->AddCollider(targetRect, coltype, callback, { 0,0 }, true);


		if (assignToCollider)
		{
			p->IOwnTheCollider = true;
			p->collider->owner = p;
		}
			
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


bool j1Particles::Load(pugi::xml_node& node)
{
	for (auto particleNode = node.child("particle"); particleNode; particleNode = particleNode.next_sibling("particle"))
	{
		std::string pName = particleNode.child("name").attribute("value").as_string();
		auto colliderNode = particleNode.child("collider");
		bool hasCollider = colliderNode.child("has_collider").attribute("value").as_bool(); 
		COLLIDER_TYPE cType = COLLIDER_NONE; 
		if (hasCollider == true)
		{
			std::string colliderTypeString = colliderNode.child("collider_type").attribute("value").as_string(); 
			if (colliderTypeString == "collider_enemy_shot")
				cType = COLLIDER_ENEMY_SHOT;
			else if (colliderTypeString == "collider_shot")
				cType = COLLIDER_SHOT;
			else if (colliderTypeString == "collider_presential")
				cType = COLLIDER_PRESENTIAL; 
		}
		SDL_RendererFlip flip = SDL_FLIP_NONE; 
		std::string renderFlipString = particleNode.child("render_flip").attribute("value").as_string();
		if (renderFlipString == "horizontal")
			flip = SDL_FLIP_HORIZONTAL; 
		float angle = particleNode.child("angle").attribute("value").as_double();
		float scale = particleNode.child("scale").attribute("value").as_float();
		auto pivotNode = particleNode.child("pivot");
		iPoint pivot = iPoint(pivotNode.attribute("x").as_int(), pivotNode.attribute("y").as_int()); 
		float currentAnimFrame = particleNode.child("current_anim_frame").attribute("value").as_float();
		auto posNode = particleNode.child("position");
		iPoint position = iPoint(posNode.attribute("x").as_int(), posNode.attribute("y").as_int());
		auto speedNode = particleNode.child("speed");
		fPoint speed = fPoint(speedNode.attribute("x").as_int(), speedNode.attribute("y").as_int());
		uint32 actualLife = particleNode.child("actual_life_time").attribute("value").as_uint();
		bool fxPlayed = particleNode.child("fx_played").attribute("value").as_bool();
		float parallaxSpeed = particleNode.child("parallax_speed").attribute("value").as_float();
		bool useCameraScale = particleNode.child("use_camera_scale").attribute("value").as_bool();
		bool onScreen = particleNode.child("on_screen").attribute("value").as_bool();
		bool toDelete = particleNode.child("to_delete").attribute("value").as_bool();


		// serious stuff xd
		uint colliderCallbackID = particleNode.child("collider_entity_callback_ID").attribute("value").as_uint();
		bool colliderOwner = particleNode.child("collider_owner").attribute("value").as_bool();
		j1Entity* callback = App->entityFactory->GetEntityFromID(colliderCallbackID); 
		
		Particle* part = AddParticleRet(pName, position.x, position.y, callback, colliderOwner, cType, speed, 0U, flip, angle, pivot.x, pivot.y, scale, parallaxSpeed, useCameraScale, onScreen);
		part->to_delete = toDelete; 

		// finally set the current frame and the delta time 
		part->anim.SetCurrentFrame(currentAnimFrame); 
		part->born = SDL_GetTicks() - actualLife; 
	}

	return true; 
}


bool j1Particles::Save(pugi::xml_node& node) const
{
	for (const auto& particle : active)
		particle->Save((pugi::xml_node&)node.append_child("particle"));

	return true;
}

bool Particle::Save(pugi::xml_node& node) const
{

	node.append_child("name").append_attribute("value") = name.c_str();

	auto colliderNode = node.append_child("collider"); 
	colliderNode.append_child("has_collider").append_attribute("value") = hasCollider; 

	if (hasCollider == true)
	{
		switch (collider->type)
		{
		case COLLIDER_SHOT:
			colliderNode.append_child("collider_type").append_attribute("value") = "collider_shot";
			break;
		case COLLIDER_ENEMY_SHOT:
			colliderNode.append_child("collider_type").append_attribute("value") = "collider_enemy_shot";
			break;
		case COLLIDER_PRESENTIAL:
			colliderNode.append_child("collider_type").append_attribute("value") = "collider_presential";
			break;
		default:
			break;
		}

	}

	switch (renderFlip)
	{
	case SDL_FLIP_NONE:
		node.append_child("render_flip").append_attribute("value") = "none"; 
		break; 
	case SDL_FLIP_HORIZONTAL:
		node.append_child("render_flip").append_attribute("value") = "horizontal";
		break;
	default:
		break;
	}

	node.append_child("angle").append_attribute("value") = angle;

	node.append_child("scale ").append_attribute("value") = scale;

	auto pivotNode = node.append_child("pivot"); 
	pivotNode.append_attribute("x") = pivot.x; 
	pivotNode.append_attribute("y") = pivot.y;

	node.append_child("current_anim_frame").append_attribute("value") = anim.GetCurrentFloatFrame(); 

	node.append_child("fx").append_attribute("value") = fx.c_str();

	auto posNode = node.append_child("position");
	posNode.append_attribute("x") = position.x;
	posNode.append_attribute("y") = position.y;

	auto speedNode = node.append_child("speed");
	speedNode.append_attribute("x") = speed.x;
	speedNode.append_attribute("y") = speed.y;

	node.append_child("actual_life_time").append_attribute("value") = life; 

	node.append_child("fx_played").append_attribute("value") = fx_played;

	node.append_child("parallax_speed").append_attribute("value") = parallaxSpeed;

	node.append_child("use_camera_scale").append_attribute("value") = useCameraScale;

	node.append_child("on_screen").append_attribute("value") = onScreen;

	node.append_child("to_delete").append_attribute("value") = to_delete;

	node.append_child("collider_owner").append_attribute("value") = IOwnTheCollider;

	// related entities: the collider has a callback (enemy or weapon) and an owner (myself)
	if (hasCollider == true && collider && collider->callback)
	{
		uint colliderCallbackID = collider->callback->ID;
		node.append_child("collider_entity_callback_ID").append_attribute("value") = collider->callback->ID; 
	}


	return true;
}



