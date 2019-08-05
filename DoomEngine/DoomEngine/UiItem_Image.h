#ifndef UIITEM_IMAGE_H
#define UIITEM_IMAGE_H

#include "UiItem.h"
#include "p2Point.h"
#include "j1Textures.h"
#include "p2Log.h"

struct SDL_Texture;

class UiItem_Image : public UiItem
{

public:
	UiItem_Image(iPoint position, const SDL_Rect* section, std::string& name, UiItem* const parent, bool isTabbable = false, SDL_Texture* specialTex = nullptr, float spriteScale = 0.0f);
	UiItem_Image(iPoint position, const SDL_Rect* section, UiItem* const parent);
	//UiItem_Image(SDL_Rect hitBox, const Animation& section, UiItem*const parent, p2Point<int> pivot = { 0,0 });
	~UiItem_Image();
	void Draw();
	void CleanUp();

private:
	SDL_Texture* specialTex = nullptr; 
	friend class UiItem_Bar; 
};


#endif
