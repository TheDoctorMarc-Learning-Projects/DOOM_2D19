#ifndef ITEM_UI_H
#define ITEM_UI_H
#include "SDL/include/SDL.h"
#include <list>
#include "p2Point.h"
#include <string>
#include "j1Scene.h"

enum UI_STATES
{
	IDLE,
	HOVER,
	CLICK,
	DRAG,
	MAX_STATES,
};

enum GUI_TYPES
{
	BAR,
	BUTTON,
	IMAGE,
	LABEL,
	CHECKBOX,
	FACE,
	UNKNOWN
};

class UiItem
{
 
public:
	UiItem(std::string name, sceneTypeGUI myscene); 
	UiItem(const iPoint pos, UiItem* const parent);
	UiItem(const iPoint pos, std::string name, UiItem* const parent);
	UiItem(UiItem* const parent);
	~UiItem();
	virtual void Draw() {};
	void DebugDraw(); 
	void DrawChildren(); 

	void AssignParentChild(UiItem* parent) {
		this->parent = parent; 
		parent->children.push_back(this); 
	}

	void MoveWithMouse(iPoint mousePos);

	void SetPos(iPoint pos) {
		hitBox.x = pos.x; 
		hitBox.y = pos.y; 
	}

	void SetOriginPos() {

		SetPos(originPos); 

		if (children.size() > 0)
			for (auto item : children)
				item->SetOriginPos();
	}

	iPoint GetPos() const { return iPoint(hitBox.x, hitBox.y); }; 

	virtual void CleanUp() {}; 
	 
	virtual void OnClickDown() {};
	virtual void OnHover() {};
	virtual void OnDrag() {};
	virtual void OnHoverExit() {};
	virtual void OnClickUp() {};

public: 
	std::string name;
	UiItem* parent = nullptr;
	// TODO: have a list of childrens. The global ui draw calls the canvas draw, then canvas calls children, etc etc etc 
	std::list<UiItem*> children;
	UI_STATES state = IDLE;
	GUI_TYPES guiType = UNKNOWN;
	SDL_Rect hitBox = { 0,0,0,0 };
	SDL_Rect section = { 0,0,0,0 };
	iPoint textureDimensions = iPoint(0, 0);
	iPoint originPos = iPoint(0, 0); 
	float scaleFactor = 0.F;
	uint mouseState = 0; 

	bool slidable = false;
	bool draggable = false; 
	bool tabbable = false;
	bool enable = true;
	bool hide = false;
	bool to_delete = false;
	bool useCamera = true;
	bool selected = false;
	bool focusable = false; 
	bool accionable = false; 
 
	void(*function) (UiItem*) = nullptr;
	std::string functionName = "empty"; 
	SceneState targetScene = SceneState::NO_SCENE; 
	sceneTypeGUI targetSceneGui = sceneTypeGUI::NO_SCENE;
	sceneTypeGUI myScene = sceneTypeGUI::NO_SCENE;

	/*virtual void DoLogicClicked() {};
	virtual void DoLogicClicked(std::string& functionName) {};
	virtual void DoLogicHovered() {};
	virtual void DoLogicAbandoned() {};
	virtual void CleanUp() {};*/
};

#endif