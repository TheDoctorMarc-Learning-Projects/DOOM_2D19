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
	UiItem(std::string name); 
	UiItem(const iPoint& pos, UiItem* const parent);
	UiItem(const iPoint& pos, std::string& name, UiItem* const parent);
	UiItem(UiItem* const parent);
	~UiItem();
	virtual void Draw() {};
	void DebugDraw(); 
	void DrawChildren(); 

	void AssignParentChild(UiItem* parent) {
		this->parent = parent; 
		parent->children.push_back(this); 
	}

	virtual void CleanUp() {}; 
	//virtual bool Update(float dt){};

	//void Draw_Cursor(float dt);

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

	bool slidable = false;
	bool tabbable = false;
	bool enable = true;
	bool hide = false;
	bool to_delete = false;
	bool useCamera = true;
	bool selected = false;
	

	// TODO: encapsulate this in a "functionData" struct 
	void(*function) (UiItem*) = nullptr;
	bool hasExecutedFunction = false; 
	sceneTypeGUI targetScene;

	/*virtual void DoLogicClicked() {};
	virtual void DoLogicClicked(std::string& functionName) {};
	virtual void DoLogicHovered() {};
	virtual void DoLogicAbandoned() {};
	virtual void CleanUp() {};*/
};

#endif