#include "j1EntityLootCoin.h"
#include "j1EntityFactory.h"
#include "j1Gui.h"
#include <string>   

j1EntityLootCoin::j1EntityLootCoin(float posX, float posY, LOOT_TYPE loot_type, std::string name, bool classicFigure) :j1EntityLoot(posX, posY, loot_type, name)
{
	this->name = name;
 


	size.create(16, 23);

	// adjust Y pos so it its just on top of floor: 
	position.y += (32 - size.y);  // 32 equals tiled tile height; 
	position.x += (32 - size.x); 

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

	if (App->entityFactory->player->godMode == true)    // cheaters don't deserve my coins >:( 
		return; 

	   // UiItem_Label* myLabel = (UiItem_Label*)App->gui->GetCanvasItemByName(name + "Label"); 

		/*if (myLabel != nullptr)
		{*/
			/*int coins = std::stoi(myLabel->text);
			coins += 1;*/

			App->audio->PlayFx("figurePickUp", 0, true, 1.f, 0.15f, 0.15f);
			//App->gui->UpDateInGameUISlot(name + "Label", coins, section);
			//App->gui->UpdateDeathTimer((int)((float)timeBonus * App->entityFactory->currentDifficultyMultiplier.deathTimerUpdate));

			to_delete = true;
		/*}
		else
			assert(false); */

}