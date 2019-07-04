#include "j1EntityFactory.h"
#include "LootWeaponMaps.h"
#include "j1Enemy.h"

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
		this->weaponData.offsetRotated = iPoint(30, 25); 

		break;

	case WEAPON_TYPE::MACHINE_GUN:

		size.create(54, 16);
		section = { 0, 24, size.x, size.y };

		// offset form player (when player is holding the weapon)
		this->weaponData.offsetFromPlayer = fPoint(21, 15);
		this->weaponData.tipPosDisplacement = 3 * spriteScale;
		this->weaponData.offsetRotated = iPoint(26, 24);

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



void j1EntityLootWeapon::Shoot(j1KeyState state)
{

	if (state == KEY_REPEAT && weaponData.FiringType != firingType::AUTO)
		return; 


	static uint lastTimeShoot = 0; 

	float ShotsInAMilisec = (float)weaponData.cadence / 60000.f;   // per minute / (60 (sec) * 1000 (milisec))
	float MiliSecShotTIme = 1.f / ShotsInAMilisec; 

	// TODO: auto weapons key repeat 


	if (SDL_GetTicks() > lastTimeShoot + (uint)(int)MiliSecShotTIme)    // TODO later on: MAXBULLETS, bullet functionality discount and prevent firing when 0 bullets etc
	{
		lastTimeShoot = SDL_GetTicks();

		SDL_RendererFlip shotFlip = SDL_FLIP_NONE;   // TODO: all particles sprites to the right by default, to match the weapon sprites and simplify this 
		iPoint weaponTipPos = iPoint(0, 0);

		if (pointingDir == POINTING_DIR::LEFT)
			SDL_RendererFlip shotFlip = SDL_FLIP_HORIZONTAL;

		if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->idle || App->entityFactory->player->currentAnimation == &App->entityFactory->player->run)
		{
			if (pointingDir == POINTING_DIR::LEFT)
			{
				weaponTipPos = iPoint(position.x, position.y + weaponData.tipPosDisplacement);
			}

			else if (pointingDir == POINTING_DIR::RIGHT)
			{
				weaponTipPos = iPoint(position.x + collider->rect.w, position.y + weaponData.tipPosDisplacement);
			}


			
			Particle* shotFire = App->particles->AddParticleRet(name + "ShotFire", weaponTipPos.x, weaponTipPos.y, this, COLLIDER_SHOT, { 0,0 }, 0U,
				flip, spriteRotation);   // just visual 


			// adjust the particle pos so that it appears centered at weapons tip 

			shotFire->position.y -= shotFire->collider->rect.h / 2;
			if (pointingDir == POINTING_DIR::LEFT)
				shotFire->position.x -= shotFire->collider->rect.w;
		}
		else
		{
			double alpha = spriteRotation; 
			weaponTipPos = iPoint(position.x, position.y); 


			Particle * shotFire = App->particles->AddParticleRet(name + "ShotFire", weaponTipPos.x, weaponTipPos.y, this, COLLIDER_SHOT, { 0,0 }, 0U,
				flip, alpha);   // just visual 


			if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimUp)
			{
				if (pointingDir == POINTING_DIR::LEFT)
				{
					weaponTipPos += iPoint(collider->rect.w - weaponData.offsetRotated.x - shotFire->collider->rect.w, -weaponData.offsetRotated.y);

					shotFire->angle = 45; 
				}
				else if (pointingDir == POINTING_DIR::RIGHT)
				{
					weaponTipPos += iPoint(weaponData.offsetRotated.x, -weaponData.offsetRotated.y);
				}
			}
			else if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimDown)

			{
				if (pointingDir == POINTING_DIR::LEFT)
				{

					weaponTipPos += iPoint(collider->rect.w - weaponData.offsetRotated.x - shotFire->collider->rect.w, weaponData.offsetRotated.y - collider->rect.h / 2);

					shotFire->angle = 315;
				}

				else if (pointingDir == POINTING_DIR::RIGHT)
				{
					weaponTipPos += iPoint(weaponData.offsetRotated.x, weaponData.offsetRotated.y - collider->rect.h / 2);  // check this out its really hardcoded
				}
			}

			shotFire->position = weaponTipPos; 
			

		}

		// the proper shot:: 


		/*if (weaponData.launchesProjectile)
			App->particles->AddParticle(name + "Shot", this, weaponTipPos.x, weaponTipPos.y, COLLIDER_NONE);  // the proper projectile with callback (this) that will do damage
		else*/
			CalculateStrike(); 
	
	}
	
	
}



void j1EntityLootWeapon::CalculateStrike()
{
	fPoint speed = fPoint(0, 0); 

	if (pointingDir == RIGHT)
	{

		if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimUp)
		{
			speed = fPoint(simulBulletSpeed, -simulBulletSpeed); 
		}
		else if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimDown)
		{
			speed = fPoint(simulBulletSpeed, simulBulletSpeed);
		}
		else
			speed = fPoint(simulBulletSpeed, 0);

	}
	else if (pointingDir == LEFT)
	{
		if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimUp)
		{
			speed = fPoint(-simulBulletSpeed, -simulBulletSpeed);
		}
		else if (App->entityFactory->player->currentAnimation == &App->entityFactory->player->aimDown)
		{
			speed = fPoint(-simulBulletSpeed, simulBulletSpeed);
		}
		else
			speed = fPoint(-simulBulletSpeed, 0);
	}


	Collider* shot = App->collision->AddCollider({(int)position.x, (int)position.y, 10, 10}, COLLIDER_TYPE::COLLIDER_SHOT, this, speed, true); // add a mini colllider to simulate bullet 
	//shot->SetPos(position.x, position.y); 


}


void j1EntityLootWeapon::OnCollision(Collider* c1, Collider* c2)
{

	if (c2->type == COLLIDER_ENEMY)
	{
		App->entityFactory->DoDamagetoEntity(c2->callback, weaponData.damage); 

		c1->to_delete = true; // delete the shot --> TODO: delete out of screen 
	}

}