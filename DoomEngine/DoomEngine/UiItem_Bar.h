#ifndef UIITEM_BAR_H
#define UIITEM_BAR_H

#include "UiItem.h"
#include "UiItem_Image.h"
#include "UiItem_Label.h"
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include "j1Textures.h"

class UiItem_Image;


class UiItem_Bar :public UiItem
{
public:
	UiItem_Bar(iPoint position, iPoint thumbOffset, std::string name, std::string functionName, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* sectioHover, 
		UiItem*const parent, float Spritescale, std::string labelText, std::string valueLabelText, SDL_Color textColor, SDL_Color valueColor, TTF_Font * font);
	~UiItem_Bar();

private:
	UiItem_Image* bar = nullptr;
	SDL_Rect idleRect;
	SDL_Rect hoverRect;
	iPoint thumbOffset; 

	uint getThumbMaxPos() const 
	{
		float value = hitBox.x + hitBox.w - thumbOffset.x - thumb->hitBox.w;
		return value; 
	}

public: 
	float getThumbTravelFactor() const
	{
		float travel = thumb->GetPos().x - thumb->originPos.x;
		float maxTravel = getThumbMaxPos() - thumb->originPos.x; 
		float value = travel / maxTravel;
		return value; 
	}

public: 
	UiItem_Image* thumb = nullptr;
	UiItem_Label* textLabel = nullptr; 
	UiItem_Label* valueLabel = nullptr; 

	 void OnDrag(iPoint mousePos);
	 void OnHover();
	 void OnHoverExit();
	 void OnClickDown(iPoint mousePos); 
	 void OnClickUp(); 




};

#endif // UIITEM_BAR_H

