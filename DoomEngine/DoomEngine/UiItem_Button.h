#ifndef UIITEM_BUTTON_H
#define UIITEM_BUTTON_H

#include "UiItem.h"
#include "p2Point.h"
#include "j1Scene.h"
#include "j1Textures.h"
#include "UiItem_Label.h"
#include <string>

class UiItem_Button :public UiItem
{
public:
	UiItem_Button(iPoint position, std::string functionName, std::string name, std::string text, SDL_Color color, TTF_Font * font,  UiItem* const parent, float spriteScale, SceneState targetScene = SceneState::NO_SCENE);
	void OnClickDown();
	void OnHover();
	void OnHoverExit(); 
	void OnClickUp();
	void Draw();


private: 
	UiItem_Label* textLabel = nullptr; 
	SDL_Color defaultLabelColor; 
};

#endif