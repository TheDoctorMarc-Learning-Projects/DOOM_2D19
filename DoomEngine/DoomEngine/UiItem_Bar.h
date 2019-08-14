#ifndef UIITEM_BAR_H
#define UIITEM_BAR_H

#include "UiItem.h"
#include "p2Point.h"
#include "j1Textures.h"

class UiItem_Image;

class UiItem_Bar :public UiItem
{
public:
	UiItem_Bar(iPoint position, std::string name, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* image_idle, const SDL_Rect* image_hover, UiItem*const parent);
	~UiItem_Bar();
	void Draw();
	float GetBarValue();

private:
	UiItem_Image* image_bar = nullptr;
	UiItem_Image* bar = nullptr;
	UiItem_Image* thumb = nullptr;
	SDL_Rect section;
	SDL_Rect image_idle;
	SDL_Rect image_hover;
	std::string name;
	bool thumbReposition = false;

public: 
	 void OnDrag();
	 void OnClickUp();

	void CleanUp();


};

#endif // UIITEM_BAR_H

