#include "j1EntityLootHealth.h"
#include "j1EntityFactory.h"

j1EntityLootHealth::j1EntityLootHealth(float posX, float posY, LOOT_TYPE loot_type, std::string name) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name;

	size.create(29, 19);

	// adjust Y pos so it its just on top of floor: 
	position.y += (32 - size.y);  // 32 equals tiled tile height; 

	section = {44, 50, size.x, size.y};

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y);

}


void j1EntityLootHealth::OnPickUp()
{
	 App->audio->PlayFx("itemPickUp", 0, false, 1.f);

	 App->entityFactory->AddLifeToPlayer(0.25f);
	 
	 to_delete = true;



}