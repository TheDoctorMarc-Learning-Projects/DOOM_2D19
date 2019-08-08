#include "j1EntityLootArmor.h"
#include "j1EntityFactory.h"

j1EntityLootArmor::j1EntityLootArmor(float posX, float posY, LOOT_TYPE loot_type, std::string name) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name;

	size.create(31, 17);

	// adjust Y pos so it its just on top of floor: 
	position.y += (32 - size.y);  // 32 equals tiled tile height; 
	position.x += (32 - size.x);

	section = { 0, 0, size.x, size.y };

	idle.PushBack({ 78, 52, size.x, size.y });
	idle.PushBack({ 119, 52, size.x, size.y });
	idle.speed = 2.f; 
	idle.loop = true; 

	currentAnimation = &idle; 

	collider = App->collision->AddCollider({ section.x, section.y, (int)((float)section.w*spriteScale), (int)((float)section.h* spriteScale) }, COLLIDER_TYPE::COLLIDER_LOOT, this);
	collider->SetPos(position.x, position.y);

}


void j1EntityLootArmor::OnPickUp()
{
	App->audio->PlayFx("itemPickUp", 0, false, 1.f);

	App->entityFactory->AddArmorToPlayer(0.3f); 
 
	to_delete = true;

}
