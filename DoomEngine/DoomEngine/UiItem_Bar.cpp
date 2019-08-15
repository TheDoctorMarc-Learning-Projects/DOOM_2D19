#include "UiItem_Bar.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "UiItem.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "UiItem_Image.h"
#include "j1Window.h"
#include "Brofiler/Brofiler.h"
#include "p2Point.h"
#include "p2log.h"
#include "j1Audio.h"
#include "j1EntityFactory.h"



UiItem_Bar::UiItem_Bar(iPoint position, iPoint thumbOffset, std::string name, std::string functionName, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* sectioHover,
	UiItem*const parent, float Spritescale, std::string labelText, std::string valueLabelText, SDL_Color textColor, SDL_Color valueColor, TTF_Font * font) : UiItem(position, parent)
{
	assert(parent != nullptr);


	if (Spritescale != 0.F)
		this->scaleFactor = Spritescale;
	else
		this->scaleFactor = App->gui->GetSpriteGlobalScale();


	this->section = *section;
	this->guiType = GUI_TYPES::BAR;
	this->image_idle = *section;
	this->image_hover = *sectioHover;
	this->hitBox.x = position.x;
	this->hitBox.y = position.y;
	this->name = name;
	
	// bar 
	bar = App->gui->AddImage(position, section, name, this, false, nullptr, this->scaleFactor);   
    // thumb
	this->thumbOffset = thumbOffset; 
	iPoint thumbPos(position.x + thumbOffset.x, position.y + thumbOffset.y);
	thumb = App->gui->AddImage(thumbPos, thumb_section, name, this, false, nullptr, this->scaleFactor);
	thumb->slidable = true;
	thumb->draggable = true;
	thumb->accionable = true;
	thumb->numb = false; 
	thumb->focusable = true; 

 
	// to check mouse 
	this->hitBox.w = section->w;
	this->hitBox.h = section->h;
	thumb->hitBox.w = thumb_section->w;
	thumb->hitBox.h = thumb_section->h;

	this->accionable = true;
	     
	// labels
	this->textLabel = App->gui->AddLabel(this->name + "LabelText", labelText, textColor, font, position + iPoint(-65, 0) , this, Spritescale);
	this->valueLabel = App->gui->AddLabel(this->name + "LabelValue", valueLabelText, valueColor, font, position + iPoint(hitBox.w + 20,0) , this, Spritescale);

	// Assign the function pointer 
	this->functionName = functionName;
	this->function = App->gui->GetFunctionsMap().at(functionName);
}

UiItem_Bar::~UiItem_Bar()
{
	
}

void UiItem_Bar::OnClickDown(iPoint mousePos)
{
	if (App->entityFactory->isPointInsideRect(&mousePos, &thumb->hitBox) == true)
	{
		App->gui->SetSelectedItem(thumb);
		thumb->state = CLICK;
	}
}

void UiItem_Bar::OnDrag(iPoint mousePos) 
{
	if (App->gui->GetSelectedItem() == this)
	{
		UiItem::OnDrag(mousePos);
		return; 
	}
	 
	// for the mom we asume that only horizontal ones are used
	thumb->MoveWithMouse(mousePos);
	thumb->SetPos(iPoint(thumb->GetPos().x, thumb->originPos.y)); // keep the X delta movement, but cap the Y movement to origin pos

	// check thumb inside X limits 
	if (thumb->GetPos().x > getThumbMaxPos())
		thumb->SetPos(iPoint(getThumbMaxPos(), thumb->GetPos().y)); 
	else if(thumb->GetPos().x < thumb->originPos.x)
		thumb->SetPos(iPoint(thumb->originPos.x, thumb->GetPos().y));

	// update the value label
	valueLabel->ChangeTextureIdle(std::to_string((uint)(int)(getThumbTravelFactor() * 100.f)), NULL, NULL);

	// finally call the function 
	this->function(this); 
}


void UiItem_Bar::OnHover()
{
	bar->section = image_hover; 
}

void UiItem_Bar::OnHoverExit()
{
	bar->section = image_idle;
}

void UiItem_Bar::OnClickUp()
{
	this->function(this);
}