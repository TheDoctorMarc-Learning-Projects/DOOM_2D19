#include "UiItem_Button.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "p2Point.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"

UiItem_Button::UiItem_Button(iPoint position, std::string functionName, std::string name, std::string text, SDL_Color color, TTF_Font * font, UiItem* const parent, float spriteScale, SceneState targetScene) :UiItem(position, name, parent)
{
	assert(parent != nullptr);

	// Basic info
	this->name = name; 
	this->guiType = GUI_TYPES::BUTTON;
	this->tabbable = true; 
 
	// Assign the function pointer 
	this->functionName = functionName; 
	this->function = App->gui->GetFunctionsMap().at(functionName);
	this->targetScene = targetScene; 
	this->targetSceneGui = App->scene->convertSceneTypeToGui(targetScene);

	// Create the label 
	this->textLabel = App->gui->AddLabel(this->name + "Label", text, color, font, position, this, spriteScale);
	this->defaultLabelColor = color; 

	// Generate the button hitbox according to label dimensions
	this->hitBox.w = this->textureDimensions.x = this->textLabel->textureDimensions.x;
	this->hitBox.h = this->textureDimensions.y = this->textLabel->textureDimensions.y;

}


void UiItem_Button::OnHover()
{
	SDL_Color c = { 230, 243, 4, 255 };
	this->textLabel->ChangeTextureIdle("", &c, NULL);
}

 
void UiItem_Button::OnClickDown()
{
	App->audio->PlayFx("buttonClick"); 
	this->function(this); 
	SDL_Color c = { 246, 220, 4, 255 }; 
	this->textLabel->ChangeTextureIdle("", &c, NULL);
}

void UiItem_Button::OnClickUp()
{
	this->textLabel->ChangeTextureIdle("", &defaultLabelColor, NULL);
}


void UiItem_Button::OnHoverExit()
{
	this->textLabel->ChangeTextureIdle("", &defaultLabelColor, NULL);
}


void UiItem_Button::Draw()   // not drawn for the mom, label draws itself 
{
	//App->render->BlitGui(App->gui->GetAtlas(), hitBox.x, hitBox.y, &frames[state], 0.0F);
}
