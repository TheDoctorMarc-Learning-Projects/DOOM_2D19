#include "UiItem_Image.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Input.h"

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section, std::string& name, UiItem* const parent, bool isTabbable) : UiItem(position, name, parent)
{
	this->section = *section;
	this->guiType = GUI_TYPES::IMAGE;

	this->hitBox.w = section->w;
	this->hitBox.h = section->h;

	tabbable = isTabbable;

	if (name == "tick")   // this is rather for checkbox purposes 
		this->hide = true;

	// the parent
	this->parent = parent;



	this->hitBox.x = position.x;
	this->hitBox.y = position.y;

}

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section, UiItem* const parent, SDL_Texture* newTex, UiItem_Description* myDescr) : UiItem(position, parent)
{
	this->section = *section;
	this->guiType = GUI_TYPES::IMAGE;


	this->hitBox.w = section->w;
	this->hitBox.h = section->h;


	// the parent
	this->parent = parent;



	this->hitBox.x = position.x;
	this->hitBox.y = position.y;

	// new texture for loot image
	this->newTex = newTex;


 
}

UiItem_Image::~UiItem_Image()
{
	
}

void UiItem_Image::Draw()
{
     // if !hide -> draw 
}

void UiItem_Image::CleanUp()
{
	
	if (newTex != nullptr)
	{
		//App->tex->UnLoad(newTex);
		newTex = nullptr;
	}
}
