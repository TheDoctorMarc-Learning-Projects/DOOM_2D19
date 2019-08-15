#include "UiItem_Face.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1EntityFactory.h"

UiItem_Face::UiItem_Face(iPoint position, std::string& name, UiItem* const parent, float spriteScale) : UiItem(position, name, parent)
{
	assert(parent != nullptr);
	this->guiType = GUI_TYPES::FACE;

	this->hitBox.x = position.x;
	this->hitBox.y = position.y;


	if (spriteScale != 0.F)
		this->scaleFactor = spriteScale;
	else
		this->scaleFactor = App->gui->GetSpriteGlobalScale();

	// the parent
	AssignParentChild(parent);

	FIllMaps(); 
}

void UiItem_Face::FIllMaps()
{
	// first create the data 
	death.PushBack({ 0, 517, 24, 31 }); 
	death.loop = false; 

	win.PushBack({ 26, 517, 24, 31 });
	win.loop = false;

	Anims healthlevel5; 
	healthlevel5.damaged.anim.name = "damaged"; 
	healthlevel5.damaged.anim.PushBack({ 0, 452, 24, 31}); 
	healthlevel5.damaged.anim.loop = false; 
	healthlevel5.damaged.myTimer.actionTime = 1000U; 

	healthlevel5.idle.anim.name = "idle";
	healthlevel5.idle.anim.PushBack({ 0, 195, 24, 29 });
	healthlevel5.idle.anim.PushBack({ 26, 195, 24, 29 });
	healthlevel5.idle.anim.PushBack({ 52, 195, 24, 29 });
	healthlevel5.idle.anim.PushBack({ 26, 195, 24, 29 });
	healthlevel5.idle.anim.loop = true;
	healthlevel5.idle.anim.speed = .5F;
 
	healthlevel5.kill.anim.name = "kill";
	healthlevel5.kill.anim.PushBack({ 0, 355, 24, 30 });
	healthlevel5.kill.anim.loop = false;
	healthlevel5.kill.myTimer.actionTime = 1000U;

	healthlevel5.weapon.anim.name = "weapon";
	healthlevel5.weapon.anim.PushBack({ 0, 485, 24, 30 });
	healthlevel5.weapon.anim.loop = false;
	healthlevel5.weapon.myTimer.actionTime = 1000U;



	Anims healthlevel4;
	healthlevel4.damaged.anim.name = "damaged";
	healthlevel4.damaged.anim.PushBack({ 26, 452, 24, 31 });
	healthlevel4.damaged.anim.loop = false;
	healthlevel4.damaged.myTimer.actionTime = 1000U;

	healthlevel4.idle.anim.name = "idle";
	healthlevel4.idle.anim.PushBack({ 0, 226, 24, 29 });
	healthlevel4.idle.anim.PushBack({ 26, 226, 24, 29 });
	healthlevel4.idle.anim.PushBack({ 52, 226, 24, 29 });
	healthlevel4.idle.anim.PushBack({ 26, 226, 24, 29 });
	healthlevel4.idle.anim.loop = true;
	healthlevel4.idle.anim.speed = .5F;

	healthlevel4.kill.anim.name = "kill";
	healthlevel4.kill.anim.PushBack({ 26, 355, 24, 30 });
	healthlevel4.kill.anim.loop = false;
	healthlevel4.kill.myTimer.actionTime = 1000U;

	healthlevel4.weapon.anim.name = "weapon";
	healthlevel4.weapon.anim.PushBack({ 26, 485, 24, 30 });
	healthlevel4.weapon.anim.loop = false;
	healthlevel4.weapon.myTimer.actionTime = 1000U;



	Anims healthlevel3;
	healthlevel3.damaged.anim.name = "damaged";
	healthlevel3.damaged.anim.PushBack({ 52, 452, 24, 31 });
	healthlevel3.damaged.anim.loop = false;
	healthlevel3.damaged.myTimer.actionTime = 1000U;

	healthlevel3.idle.anim.name = "idle";
	healthlevel3.idle.anim.PushBack({ 0, 257, 24, 29 });
	healthlevel3.idle.anim.PushBack({ 26, 257, 24, 29 });
	healthlevel3.idle.anim.PushBack({ 52, 257, 24, 29 });
	healthlevel3.idle.anim.PushBack({ 26, 257, 24, 29 });
	healthlevel3.idle.anim.loop = true;
	healthlevel3.idle.anim.speed = .5F;

	healthlevel3.kill.anim.name = "kill";
	healthlevel3.kill.anim.PushBack({ 52, 355, 24, 30 });
	healthlevel3.kill.anim.loop = false;
	healthlevel3.kill.myTimer.actionTime = 1000U;

	healthlevel3.weapon.anim.name = "weapon";
	healthlevel3.weapon.anim.PushBack({ 52, 485, 24, 30 });
	healthlevel3.weapon.anim.loop = false;
	healthlevel5.weapon.myTimer.actionTime = 1000U;



	Anims healthlevel2;
	healthlevel2.damaged.anim.name = "damaged";
	healthlevel2.damaged.anim.PushBack({ 78, 452, 24, 31 });
	healthlevel2.damaged.anim.loop = false;
	healthlevel2.damaged.myTimer.actionTime = 1000U;

	healthlevel2.idle.anim.name = "idle";
	healthlevel2.idle.anim.PushBack({ 0, 289, 24, 29 });
	healthlevel2.idle.anim.PushBack({ 26, 289, 24, 29 });
	healthlevel2.idle.anim.PushBack({ 52, 289, 24, 29 });
	healthlevel2.idle.anim.PushBack({ 26, 289, 24, 29 });
	healthlevel2.idle.anim.loop = true;
	healthlevel2.idle.anim.speed = .5F;

	healthlevel2.kill.anim.name = "kill";
	healthlevel2.kill.anim.PushBack({ 78, 355, 24, 30 });
	healthlevel2.kill.anim.loop = false;
	healthlevel2.kill.myTimer.actionTime = 1000U;

	healthlevel2.weapon.anim.name = "weapon";
	healthlevel2.weapon.anim.PushBack({ 78, 485, 24, 30 });
	healthlevel2.weapon.anim.loop = false;
	healthlevel2.weapon.myTimer.actionTime = 1000U;



	Anims healthlevel1;
	healthlevel1.damaged.anim.name = "damaged";
	healthlevel1.damaged.anim.PushBack({ 104, 452, 24, 31 });
	healthlevel1.damaged.anim.loop = false;
	healthlevel1.damaged.myTimer.actionTime = 1000U;

	healthlevel1.idle.anim.name = "idle";
	healthlevel1.idle.anim.PushBack({ 0, 322, 24, 29 });
	healthlevel1.idle.anim.PushBack({ 26, 322, 24, 29 });
	healthlevel1.idle.anim.PushBack({ 52, 322, 24, 29 });
	healthlevel1.idle.anim.PushBack({ 26, 322, 24, 29 });
	healthlevel1.idle.anim.loop = true;
	healthlevel1.idle.anim.speed = .5F;

	healthlevel1.kill.anim.name = "kill";
	healthlevel1.kill.anim.PushBack({ 104, 355, 24, 30 });
	healthlevel1.kill.anim.loop = false;
	healthlevel1.kill.myTimer.actionTime = 1000U;

	healthlevel1.weapon.anim.name = "weapon";
	healthlevel1.weapon.anim.PushBack({ 104, 485, 24, 30 });
	healthlevel1.weapon.anim.loop = false;
	healthlevel1.weapon.myTimer.actionTime = 1000U;


	// then fill the maps 
	healthlevel5.karmaMap =
	{
		{ "idle", healthlevel5.idle},
		{ "damaged", healthlevel5.damaged},
		{ "kill", healthlevel5.kill},
		{ "weapon", healthlevel5.weapon},
	}; 

	healthlevel4.karmaMap =
	{
		{ "idle", healthlevel4.idle},
		{ "damaged", healthlevel4.damaged},
		{ "kill", healthlevel4.kill},
		{ "weapon", healthlevel4.weapon},
	};

	healthlevel3.karmaMap =
	{
		{ "idle", healthlevel3.idle},
		{ "damaged", healthlevel3.damaged},
		{ "kill", healthlevel3.kill},
		{ "weapon", healthlevel3.weapon},
	};

	healthlevel2.karmaMap =
	{
		{ "idle", healthlevel2.idle},
		{ "damaged", healthlevel2.damaged},
		{ "kill", healthlevel2.kill},
		{ "weapon", healthlevel2.weapon},
	};

	healthlevel1.karmaMap =
	{
		{ "idle", healthlevel1.idle},
		{ "damaged", healthlevel1.damaged},
		{ "kill", healthlevel1.kill},
		{ "weapon", healthlevel1.weapon},
	};

	animMap = {
		{5U, healthlevel5},
		{4U, healthlevel4},
		{3U, healthlevel3},
		{2U, healthlevel2},
		{1U, healthlevel1},
	}; 
 

	// initialize the face to the default full health idle anim
	currentAnimation.anim = animMap.at(5U).karmaMap.at("idle").anim;
 
	

}

UiItem_Face::~UiItem_Face()
{

}

void UiItem_Face::Draw()
{
	uint ms = currentAnimation.myTimer.timer.Read(); 
	uint ms2 = currentAnimation.myTimer.actionTime; 

	// if the kil, weapon etc state is over, go to idle anim
	if (toUpdate == true)
		if (currentAnimation.myTimer.timer.Read() > currentAnimation.myTimer.actionTime)
		{
			SetCurrentAnim("idle");
			toUpdate = false; 
		}
		
	
	App->render->Blit(App->gui->GetAtlas(), hitBox.x, hitBox.y, &currentAnimation.anim.GetCurrentFrame(), 0.0F, SDL_FLIP_NONE, scaleFactor);

}

 

void UiItem_Face::SetCurrentAnim(std::string karma)
{
	// First of all discard extreme cases where no calculations are needed
	
	if (karma == "win")
	{
		currentAnimation.anim = win;
		return; 
	}
	if (App->entityFactory->playerAlive == false || (App->entityFactory->playerAlive == true && App->entityFactory->player->godMode == true))
		return; 
	if (karma == "death")
	{
		currentAnimation.anim = death;
		return; 
	}
	if (App->entityFactory->player->life <= 0)
		return; 


	// calculations 
	float playerHealthPercent = 100.F; 
	if(App->entityFactory->playerAlive == true)
		playerHealthPercent = (App->entityFactory->player->life * 100) / App->entityFactory->player->maxLife;    // get the player current health percentatge 
	float healthLevelModule = 100 / animMap.size();                                                                // divide 100% by number of health levels (5) to get the % each divison has 
	float actualHealthLevel = playerHealthPercent / healthLevelModule;                       // translate the % to a health level value eg 75% / 20% = 3.75
	uint roundedHealthLevel = (uint)(int)(float)std::ceil((double)(actualHealthLevel));      // round UP the previous number to get the real uint level

	// assignment
	currentAnimation = animMap.at(roundedHealthLevel).karmaMap.at(karma);    // get the anim by the animSet by health level, and, inside, the specific anim by karma string 
	currentAnimation.myTimer.timer.Start();   // set the timer 
	

	toUpdate = true; 
}

void UiItem_Face::CleanUp()
{

	for (auto& anims : animMap)
		anims.second.karmaMap.clear(); 

	animMap.clear(); 
 
}
