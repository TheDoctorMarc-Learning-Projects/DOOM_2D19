#include "UiItem_Image.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Map.h"

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section, std::string& name, UiItem* const parent, bool isTabbable, SDL_Texture* specialTex, float spriteScale,
	std::string newTextureName) : UiItem(position, name, parent)
{
	assert(parent != nullptr);

	this->section = *section;
	this->guiType = GUI_TYPES::IMAGE;

	this->hitBox.w = section->w;
	this->hitBox.h = section->h;


	this->hitBox.x = position.x;
	this->hitBox.y = position.y;

	tabbable = isTabbable;
	this->numb = true; 

	if (specialTex != nullptr)
	{
		this->specialTex = specialTex;
		this->newTextureName = newTextureName; 
	}
		
	if (spriteScale != 0.F)
		this->scaleFactor = spriteScale; 
	else
		this->scaleFactor = App->gui->GetSpriteGlobalScale();


}

UiItem_Image::UiItem_Image(iPoint position, const SDL_Rect* section, UiItem* const parent) : UiItem(position, parent)
{
	assert(parent != nullptr);

	this->section = *section;
	this->guiType = GUI_TYPES::IMAGE;


	this->hitBox.w = section->w;
	this->hitBox.h = section->h;


	this->hitBox.x = position.x;
	this->hitBox.y = position.y;

 
}

UiItem_Image::~UiItem_Image()
{
	
}

void UiItem_Image::Draw()
{ 
	if (specialTex != nullptr)
		App->render->Blit(specialTex, hitBox.x, hitBox.y, &this->section, 0.0F, SDL_FLIP_NONE, scaleFactor);
	else
		App->render->Blit(App->gui->GetAtlas(), hitBox.x, hitBox.y, &this->section, 0.0F, SDL_FLIP_NONE, scaleFactor);
}

void UiItem_Image::ReAssignSpecialTexture()
{
	specialTex = App->map->entityTextureMap.at(newTextureName);
}

void UiItem_Image::CleanUp()
{
	if (specialTex != nullptr)
		specialTex = nullptr; 
}


void UiItem_Image::OnDrag(iPoint mousePos)
{
	if (parent && parent->guiType == BAR)
		parent->OnDrag(mousePos); 
}
void UiItem_Image::OnHover()
{
	if (parent && parent->guiType == BAR)
		parent->OnHover();

}
void UiItem_Image::OnHoverExit()
{
	if (parent && parent->guiType == BAR)
		parent->OnHoverExit();

}

void UiItem_Image::OnClickDown(iPoint mousePos)
{
	if (parent && parent->guiType == BAR)
		parent->OnClickDown(mousePos);
}

void UiItem_Image::OnClickUp()
{
	if (parent && parent->guiType == BAR)
		parent->OnClickUp();

}
