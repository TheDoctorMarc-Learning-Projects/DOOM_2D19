#include "UiItem_Label.h"
#include "UiItem.h"

#include "j1App.h"
#include "j1Fonts.h"
#include "j1Scene.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Gui.h"
#include <assert.h>


#include "j1EntityFactory.h"



UiItem_Label::UiItem_Label(std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem*const parent) :UiItem(position, parent)
{
	assert(parent != nullptr);

	texture = App->font->Print(text.data(), color, font);
	this->guiType = GUI_TYPES::LABEL;
	this->text = text;
	this->color = color;
	this->font = font;


	if (texture)
		SDL_QueryTexture(texture, NULL, NULL, &textureDimensions.x, &textureDimensions.y);

	// the parent
	AssignParentChild(parent);

}

UiItem_Label::~UiItem_Label()
{
	
}

//bool UiItem_Label::ChangeTextureHover(const std::string * textHover, const SDL_Color * color, const TTF_Font * font)
//{
//	this->guiType = GUI_TYPES::LABEL;
//	typewriter_text = text;
//	this->text = "";
//	this->color = color;
//	this->font = font;
//	texture = App->font->Print(this->text.data(), color, font);
//
//	if (texture)
//		SDL_QueryTexture(texture, NULL, NULL, &textureDimensions.x, &textureDimensions.y);
//
//	// the parent
//	this->parent = parent;
//	spawn_typewriter = true;
//	right_text = true;
//	
//}


void UiItem_Label::Draw()
{
	/*if (!hide)
			App->render->BlitGui(texture, hitBox.x, hitBox.y, NULL, speed);*/
	
}

void UiItem_Label::CleanUp()
{
	if (texture != nullptr)
	{
		App->tex->UnLoad(texture);
		texture = nullptr;
	}
}

bool UiItem_Label::ChangeTextureIdle(std::string textIdle, const SDL_Color* color, const TTF_Font* font, uint numberOfDecimals)
{
	bool ret = false;

	SDL_Color col = (color != NULL) ? *color : this->color;
	TTF_Font * f = (font != NULL) ? (TTF_Font *)font : this->font;

	SDL_Texture* aux = App->font->Print(textIdle.data(), col, f);
	if(aux != nullptr)
		assert(aux != nullptr);

	if (this->texture != nullptr)
	{
		App->tex->UnLoad(this->texture);
		this->texture = nullptr;
	}

	texture = aux;

	return ret;


}

void UiItem_Label::DoLogicHovered()
{
 
}



void UiItem_Label::DoLogicAbandoned()
{
 
}

