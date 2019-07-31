#ifndef ITEM_UI_H
#define ITEM_UI_H
#include "SDL/include/SDL.h"
#include <list>
#include "p2Point.h"
#include <string>


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
	UNKNOWN
};

class UiItem
{
protected:
	// UiItem* parent = NULL;
public:
	UiItem* parent = nullptr;
	// TODO:have a list of childrens. The global ui draw calls the canvas draw, then canvas calls children, etc etc etc 
	std::string name;

	UI_STATES state = IDLE;
	GUI_TYPES guiType = UNKNOWN;
	std::string name_object;
	SDL_Rect hitBox = { 0,0,0,0 };
	std::string function;
	// p2Point<int> pivot = { 0,0 };
	uint mouseButtonDown = 0;
	UiItem(const iPoint& pos, UiItem* const parent);
	UiItem(const iPoint& pos, std::string& name, UiItem* const parent);
	UiItem(const iPoint& pos, std::string& function, std::string& name, UiItem* const parent);
	UiItem(UiItem* const parent);
	~UiItem();
	virtual void Draw(const float&) {};
	virtual void CleanUp() {}; 
	//virtual bool Update(float dt){};

	//void Draw_Cursor(float dt);
	bool slidable = false;
	bool tabbable = false;
	bool enable = true;
	bool hide = false;
	bool to_delete = false;
	bool useCamera = true;
	bool selected = false;
	int scaleFactor;
	iPoint textureDimensions;
	iPoint prevTextDimension;



	/*virtual void DoLogicClicked() {};
	virtual void DoLogicClicked(std::string& functionName) {};
	virtual void DoLogicHovered() {};
	virtual void DoLogicAbandoned() {};
	virtual void CleanUp() {};*/
};

#endif