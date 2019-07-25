#include "j1EntityLootCoin.h"
 
j1EntityLootCoin::j1EntityLootCoin(float posX, float posY, LOOT_TYPE loot_type, std::string name, bool classicFigure) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name;
 


	size.create(16, 23);

	// adjust Y pos so it its just on top of floor: 
	position.y += (32 - size.y);  // 32 equals tiled tile height; 


	section.w = size.x; 
	section.h = size.y; 
	section.y = 45; 
	if (classicFigure == true)
		section.x = 22; 

	
	

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y);

}


void j1EntityLootCoin::OnPickUp()
{
	if (to_delete == false)
	{
		// TODO 1: play a pickup fx, so while it is player, the music and other fx are set to a desired lower volume 
		App->audio->PlayFx("figurePickUp", 0, true, 1.f, 0.1f, 0.1f);

		// TODO 3: add to the UI counter

		// TODO 2: destroy myself
		to_delete = true;

	}

}