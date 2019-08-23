#include "j1EnemyHellKnight.h"
#include "j1EntityFactory.h"

j1EnemyHellKnight::j1EnemyHellKnight(int posX, int posY, std::string name) : j1Enemy(posX, posY)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - texture
	entityTex = App->entityFactory->entityTextureMap.at(name);

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - data
	type = ENEMY_HELL_KNIGHT;
	this->name = name;
	powerLevel = 2U; 
	maxLife = 200;
	life = (float)maxLife;
	size.create(44, 70);
	speed = 90.33f;
	defaultSpeed = speed;
	mass = 1.2f;
	gravityFactor = DEFAULT_GRAV / mass;  // TODO: just prevent Y movement by gravity
	tileDetectionRange = 15;
	cadenceValues.melee = 1000;
	damageValues.melee = 100;
	pathType = enemyPathType::A_TO_B;
	dataAnimFx.hasSecondDeathAnim = false;
	dataAnimFx.hasSecondDeathFx = false;
	hasMaxDistanceFromOrigin = true;
	maxDistFromOrigin = 15U;
	myMeleeRange = 2U;
	hasMaxDistanceFromOrigin = false;   

	state.movement.at(1) = eMovementState::FALL;    // check this out: when TODO: load enemies in map, it should no longer be fall  
	state.path = ePathState::AWAIT; 

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - anims
	currentAnimation = &idle;
	idle.PushBack({ 25, 135, size.x, size.y });

	run.PushBack({ 18, 28, size.x + 8, size.y - 1 });
	run.PushBack({ 25, 135, size.x, size.y });
	// missing speed: Baron and Knight

	death1.PushBack({ 22, 675, size.x + 4, size.y + 3 });
	death1.PushBack({ 114, 675, size.x + 11, size.y - 6 });
	death1.PushBack({ 213, 675, size.x + 12, size.y - 16 });
	death1.PushBack({ 313, 675, size.x + 15, size.y - 34 });
	death1.PushBack({ 416, 675, size.x + 16, size.y - 40 });
	death1.PushBack({ 520, 675, size.x + 16, size.y - 40 });
	death1.PushBack({ 624, 675, size.x + 16, size.y - 40 });
	death1.speed = 2.2f;
	death1.loop = false;

	attack.PushBack({ 32, 236, size.x + 12, size.y - 4 });
	attack.PushBack({ 43, 343, size.x + 7, size.y - 2 });
	attack.PushBack({ 42, 449, size.x + 5, size.y - 8 });
	attack.PushBack({ 34, 548, size.x + 3, size.y + 1 });
	attack.speed = 2.7f;
	attack.loop = false;
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - collider
	collider = App->collision->AddCollider({ (int)position.x, (int)position.y, (int)((float)size.x * spriteScale),(int)((float)size.y * spriteScale) }, COLLIDER_TYPE::COLLIDER_ENEMY, this);
}

j1EnemyHellKnight::~j1EnemyHellKnight()
{
}


bool j1EnemyHellKnight::Move(float dt)
{
	if (App->entityFactory->IsPlayerAlive() == false) // first line prevention _> TODO: maybe it is safer to just stop the entity factory, but the player weapon wouldn't fall to the floor then 
	{
		return false;
	}

	j1Enemy::Move(dt);

	if (state.combat != eCombatState::DYING && state.combat != eCombatState::DEAD)
	{
		/*if (isPlayerOnMeleeRange() == true && state.combat != eCombatState::SHOOT && lastSpeed.IsZero() == true)
			currentAnimation = &idle;*/

		ChangeState(); 

		if (myState == BEHAVIOUR_STATE::A_TO_B)
			Go_A_to_B();
		else if(myState == BEHAVIOUR_STATE::FOLLOW)
			FallToPlayer(); 
		
		if(isPlayerOnSamePlatform() == true)
			DoMeleeAttack();
		else
		{
			if (currentAnimation == &attack)
				currentAnimation = &run; 
		}


	}


	return true;
}

void j1EnemyHellKnight::ChangeState()
{
	// player same platform, stop A to B 

	if (myState == BEHAVIOUR_STATE::A_TO_B)
		if (App->entityFactory->player->onPlatform == true && onPlatform == true && App->entityFactory->player->lastPlatform == lastPlatform)  
		{
			myState = BEHAVIOUR_STATE::FOLLOW;
			state.path = ePathState::FOLLOW_PLAYER; 
			speed = defaultSpeed;
		}

 
	if (state.path == ePathState::TEMPORAL_DEVIATION && onPlatform == true)
	{
		if (targetPlatform == nullptr)
		{
			if (App->entityFactory->player->onPlatform == true && onPlatform == true && App->entityFactory->player->lastPlatform != lastPlatform)
			{
				myState = BEHAVIOUR_STATE::A_TO_B;
			}
		}

	}
}


void j1EnemyHellKnight::FallToPlayer()
{
 
 
	// player not in same platform, follow him and fall
	bool doIt = false; 

	if (App->entityFactory->player->onPlatform == true && onPlatform == true && App->entityFactory->player->lastPlatform != lastPlatform)
	{
		if (App->entityFactory->player->lastPlatform->heightLevel < lastPlatform->heightLevel && App->entityFactory->player->lastPlatform->heightLevel != 0)   // security: only fall, and not to base floor
		{
			if (targetPlatform == nullptr)
			{
				if (App->entityFactory->player->lastPlatform->type == ENTITY_STATIC)
				{
					doIt = true;

					if (App->entityFactory->player->position.x + App->entityFactory->player->collider->rect.w / 2 >= position.x + collider->rect.w / 2)
						specificDir = iPoint(1, 0);

					else
						specificDir = iPoint(-1, 0);

					state.path = ePathState::TEMPORAL_DEVIATION;

					targetPlatform = App->entityFactory->player->lastPlatform;
				}

			}
		}


			if(doIt == false)
				myState = BEHAVIOUR_STATE::A_TO_B;

	
	}


	if (onPlatform == true && targetPlatform != nullptr && lastPlatform == targetPlatform)   // when falling to another platform, set back to A to B
	{
		targetPlatform = nullptr;
		myState = BEHAVIOUR_STATE::A_TO_B;

		resetAtoB = true;
	}
	 
}


void j1EnemyHellKnight::ResolvePathDeviation()
{
	if (myState == BEHAVIOUR_STATE::A_TO_B)
	{
		if (pointingDir == LEFT)
			pointingDir = RIGHT;
		else if (pointingDir == RIGHT)
			pointingDir = LEFT;

	}
 

 
}


bool j1EnemyHellKnight::Load(pugi::xml_node& node)
{
	j1Enemy::Load(node);

	uint lastTargetPlatformID = node.child("particular_data").child("last_target_platform_ID").attribute("value").as_uint();
	targetPlatform = (j1EntityPlatform*)App->entityFactory->GetEntityFromID(lastTargetPlatformID);

	std::string behaviourString = node.child("particular_data").child("behaviour_state").attribute("value").as_string(); 

	if (behaviourString == "A_TO_B")
		myState = BEHAVIOUR_STATE::A_TO_B; 
	else if (behaviourString == "FOLLOW")
		myState = BEHAVIOUR_STATE::FOLLOW;

	return true;
}

bool j1EnemyHellKnight::Save(pugi::xml_node& node) const
{
	j1Enemy::Save(node);

	auto myOwnData = node.append_child("particular_data");

	if (targetPlatform != nullptr)
		myOwnData.append_child("last_target_platform_ID").append_attribute("value") = targetPlatform->ID;


	auto behaviour = myOwnData.append_child("behaviour_state"); 

	switch (myState)
	{
	case BEHAVIOUR_STATE::A_TO_B:
		behaviour.append_attribute("value") = "A_TO_B"; 
		break;
	case BEHAVIOUR_STATE::FOLLOW:
		behaviour.append_attribute("value") = "FOLLOW";
		break;
	default:
		break;
	}

	return true;

}
