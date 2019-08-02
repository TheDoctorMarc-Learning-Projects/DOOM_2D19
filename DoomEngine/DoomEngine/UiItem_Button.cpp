
#include "UiItem_Button.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "p2Point.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"


// TODO: Define the function name in the XML 
UiItem_Button::UiItem_Button(iPoint position, std::string functionName, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click, const SDL_Rect* hover, sceneTypeGUI targetScene) :UiItem(position, name, parent)
{
	assert(parent != nullptr);
	frames[IDLE] = *idle;

	if (click)
		frames[CLICK] = *click;
	else
		frames[CLICK] = *idle;

	if (hover)
		frames[HOVER] = *hover;
	else
		frames[HOVER] = *idle;

	this->guiType = GUI_TYPES::BUTTON;
	hitBox.w = idle->w;
	hitBox.h = idle->h;
 
	// assign the function pointer here
	this->function = App->gui->GetFunctionsMap().at(functionName);
	this->targetScene = targetScene; 
}

 
void UiItem_Button::DoLogicClicked()
{
	hasExecutedFunction = true; 
	this->function(this); 
}

void UiItem_Button::Draw()
{
	//App->render->BlitGui(App->gui->GetAtlas(), hitBox.x, hitBox.y, &frames[state], 0.0F);
}
