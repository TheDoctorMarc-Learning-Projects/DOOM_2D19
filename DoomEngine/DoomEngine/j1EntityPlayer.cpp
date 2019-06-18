#include "j1EntityPlayer.h"
#include "j1Input.h"
#include "j1Render.h"
#include "p2Log.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1EntityFactory.h"
#include "j1Window.h"
#include <math.h>


j1EntityPlayer::j1EntityPlayer(int posX, int posY) : j1Entity(PLAYER, posX , posY, "player")
{
	
	entityTex = App->tex->Load("textures/player/player.png");
	type = PLAYER; 
	position = previousPosition = iPoint(posX, posY); 
	currentAnimation = &idle; 
	size.create(34, 53);
	idle.PushBack({18, 421, size.x, size.y}); 



	collider = App->collision->AddCollider({position.x, position.y, size.x, size.y}, COLLIDER_TYPE::COLLIDER_PLAYER, (j1Module*)this);

}

j1EntityPlayer::~j1EntityPlayer()
{
}

bool j1EntityPlayer::Start()
{

	return true;
}

bool j1EntityPlayer::PreUpdate()
{
	
	return true;
}

bool j1EntityPlayer::Update(float dt)
{

	InputMovement(dt);
	return true;
}

bool j1EntityPlayer::PostUpdate()
{

	return true;
}

/*bool j1EntityPlayer::CleanUp()
{
	
	if (entityTex != nullptr)
		App->tex->UnLoad(entityTex);


	return true;
}*/

bool j1EntityPlayer::Load(pugi::xml_node &)
{
	return true;
}

bool j1EntityPlayer::Save(pugi::xml_node &) const
{
	return true;
}



bool j1EntityPlayer::InputMovement(float dt)
{
	bool isMoving = false;

	previousPosition = position;




	return true;
}

/*
bool j1EntityPlayer::InputCombat()
{
	

	// reset combat state
	combat_state = combatState::IDLE;
	

	return true;
}
*/



