#ifndef UIITEM_BAR_H
#define UIITEM_BAR_H

#include "UiItem.h"
#include "UiItem_Image.h"
#include "p2Point.h"
#include "j1Textures.h"

class UiItem_Image;

class UiItem_Bar :public UiItem
{
public:
	UiItem_Bar(iPoint position, iPoint thumbOffset, std::string name, std::string functionName, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* sectioHover, 
		UiItem*const parent, float Spritescale);
	~UiItem_Bar();

private:
	UiItem_Image* bar = nullptr;
	SDL_Rect section;
	SDL_Rect image_idle;
	SDL_Rect image_hover;
	iPoint thumbOffset; 
	std::string name;

	uint getThumbMaxPos() const 
	{
		return hitBox.x + hitBox.w - thumbOffset.x - thumb->hitBox.w; 
	}

	int getThumbTravelFactor() const
	{
		return thumb->GetPos().x / getThumbMaxPos(); 
	}

public: 
	UiItem_Image* thumb = nullptr;

	 void OnDrag(iPoint mousePos);
	 void OnHover();
	 void OnHoverExit();
	 void OnClickDown(iPoint mousePos); 

	void CleanUp();


};

#endif // UIITEM_BAR_H

