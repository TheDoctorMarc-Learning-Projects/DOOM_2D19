#include "j1EntityLootAmmo.h"
#include "j1EntityFactory.h"

j1EntityLootAmmo::j1EntityLootAmmo(float posX, float posY, LOOT_TYPE loot_type, std::string name) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name;

	size.create(15, 7);

	// adjust Y pos so it its just on top of floor: 
	position.y += (32 - size.y);  // 32 equals tiled tile height; 

	section = { 153, 60, size.x, size.y };

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y);

}


void j1EntityLootAmmo::OnPickUp()
{
	App->audio->PlayFx("itemPickUp", 0, false, 1.f);

	App->entityFactory->AddAmmoToPlayer(0.25f); 

	to_delete = true;

}
