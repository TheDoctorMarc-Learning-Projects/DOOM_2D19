#ifndef UIITEM_BUTTON_H
#define UIITEM_BUTTON_H

#include "UiItem.h"
#include "p2Point.h"
#include "j1Scene.h"
#include "j1Textures.h"
#include <string>

class UiItem_Button :public UiItem
{
protected:
	SDL_Rect frames[MAX_STATES];
	std::string functionName;


public:
	UiItem_Button(iPoint position, std::string functionName, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click = nullptr, const SDL_Rect* hover = nullptr, 
		sceneTypeGUI targetSceneGUI = sceneTypeGUI::NO_SCENE, SceneState targetScene = SceneState::NO_SCENE);
	void DoLogicClicked();
	void Draw();
 
};

#endif