#include "j1EntityFactory.h"
#include "LootWeaponMaps.h"

j1EntityLootWeapon::j1EntityLootWeapon(float posX, float posY, LOOT_TYPE loot_type, std::string name, weaponInfo weaponData) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name; 
	useRenderFlip = true;
	defaultPointingDir = POINTING_DIR::RIGHT; 
	pointingDir = POINTING_DIR::RIGHT; 

	this->weaponData = weaponData; 
	this->weaponData.weaponState = WEAPON_STATE::AWAIT; 

	// define an offset from player, which may vary depending on weapon sprite

	 
	spriteScale = 0.65f;   // in the available sprites weapons are too big 

	switch (weaponData.weaponType)
	{
	case WEAPON_TYPE::CHAINSAW:

		size.create(62, 23); 
		section = {30, 0, size.x, size.y}; 
		
		// offset form player (when player is holding the weapon)
		this->weaponData.offsetFromPlayer = fPoint(21, 15);
		this->weaponData.tipPosDisplacement = 16 * spriteScale;

		break; 


	case WEAPON_TYPE::SHOTGUN:

		size.create(63, 12);
		section = { 147, 0, size.x, size.y };

		// offset form player (when player is holding the weapon)
		this->weaponData.offsetFromPlayer = fPoint(17, 20);
		this->weaponData.tipPosDisplacement = 2 * spriteScale;

		break;

	case WEAPON_TYPE::MACHINE_GUN:

		size.create(54, 16);
		section = { 0, 24, size.x, size.y };

		// offset form player (when player is holding the weapon)
		this->weaponData.offsetFromPlayer = fPoint(21, 15);
		this->weaponData.tipPosDisplacement = 3 * spriteScale;
	

		break;
	}

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y); 

}

 
j1EntityLootWeapon::~j1EntityLootWeapon()
{

}

void j1EntityLootWeapon::PlaceMeWithPlayer()
{
	pointingDir = App->entityFactory->player->pointingDir; 

	if(pointingDir == POINTING_DIR::RIGHT)
		position = App->entityFactory->player->position + weaponData.offsetFromPlayer;
	else if (pointingDir == POINTING_DIR::LEFT)
	{
		
		position = App->entityFactory->player->position + fPoint(App->entityFactory->player->collider->rect.w - weaponData.offsetFromPlayer.x - collider->rect.w, weaponData.offsetFromPlayer.y);
	}

	collider->SetPos(position.x, position.y); 
	lastSpeed = App->entityFactory->player->lastSpeed; 

}


void j1EntityLootWeapon::ChangeRotation(double angle)
{
	spriteRotation = angle; 

	if (angle == 0)
		rotationPivots.create(INT_MAX, INT_MAX); 
	else
	{
		rotationPivots.y = 0;

		if (pointingDir == POINTING_DIR::RIGHT)
			rotationPivots.x = weaponData.xDisplacementWhenRotated;
		else if (pointingDir == POINTING_DIR::LEFT)
			rotationPivots.x = collider->rect.w - weaponData.xDisplacementWhenRotated;

	}


}



void j1EntityLootWeapon::Shoot()
{

	// TODO: take cadence and capture last shot ticks, to add the cooldown   // TODO 2: auto weapons, get the key repeat 

	static uint lastTimeShoot = 0; 

	float ShotsInAMilisec = (float)weaponData.cadence / 60000.f;   // per minute / (60 (sec) * 1000 (milisec))
	float MiliSecShotTIme = 1.f / ShotsInAMilisec; 




	if (SDL_GetTicks() > lastTimeShoot + (uint)(int)MiliSecShotTIme)
	{
		lastTimeShoot = SDL_GetTicks();

		SDL_RendererFlip shotFlip = SDL_FLIP_NONE;   // TODO: all particles sprites to the right by default, bo matche the weapon sprites and simplify this 
		iPoint weaponTipPos = iPoint(0, 0);

		if (pointingDir == POINTING_DIR::LEFT)
		{
			weaponTipPos = iPoint(position.x, position.y + weaponData.tipPosDisplacement);
			SDL_RendererFlip shotFlip = SDL_FLIP_HORIZONTAL;
		}

		else if (pointingDir == POINTING_DIR::RIGHT)
		{
			weaponTipPos = iPoint(position.x + collider->rect.w, position.y + weaponData.tipPosDisplacement);
		}


		/*if (weaponData.launchesProjectile)
			App->particles->AddParticle(name + "Shot", this, weaponTipPos.x, weaponTipPos.y, COLLIDER_NONE);  // the proper projectile with callback (this) that will do damage
			*/
		Particle* shotFire = App->particles->AddParticleRet(name + "ShotFire", weaponTipPos.x, weaponTipPos.y, this, COLLIDER_SHOT, { 0,0 }, 0U,
			SDL_FLIP_NONE, spriteRotation);   // just visual 


		// adjust the particle pos so that it appears centered at weapons tip 

		shotFire->position.y -= shotFire->collider->rect.h / 2;
		if (pointingDir == POINTING_DIR::LEFT)
			shotFire->position.x -= shotFire->collider->rect.w;
	}
	
	
}
