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
	FACE,
	UNKNOWN
};


struct targetPosData
{
	bool toUpdatePos = false;
	iPoint targetPos = iPoint(0, 0);
	iPoint speed = iPoint(0,0); 
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

	void EnableChildren(bool enable)
	{
		if(children.size() > 0)
			for (auto& child : children)
			{
				child->enable = enable; 

				if (child->children.size() > 0)
					child->EnableChildren(enable); 
			}
				
	}

	std::list<UiItem*> GetChildrenRecursive()
	{
		std::list<UiItem*> listChildren; 

		for (const auto& child : this->children)
		{
			listChildren.push_back(child);

			if (child->children.size() > 0)
			{
				std::list<UiItem*> listGrandChildren;
				listGrandChildren = child->GetChildrenRecursive();
				listChildren.splice(listChildren.end(), listGrandChildren); 
			}
			

		}
			
		return listChildren; 
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
	 
	virtual void OnClickDown(iPoint mousePos) {};
	virtual void OnHover() {};
	virtual void OnDrag(iPoint mousePos) {
		MoveWithMouse(mousePos);
	};
	virtual void OnHoverExit() {};
	virtual void OnClickUp() {};

public: 
	std::string name;
	UiItem* parent = nullptr;
	std::list<UiItem*> children;
	UI_STATES state = IDLE;
	GUI_TYPES guiType = UNKNOWN;
	SDL_Rect hitBox = { 0,0,0,0 };
	SDL_Rect section = { 0,0,0,0 };
	iPoint textureDimensions = iPoint(0, 0);
	iPoint offsetWithMouse = iPoint(0, 0); 
	iPoint originPos = iPoint(0, 0); 
	float scaleFactor = 0.F;
	targetPosData targPosData;
	
	bool slidable = false;
	bool draggable = false; 
	bool enable = true;
	bool hide = false;
	bool to_delete = false;
	bool focusable = false; 
	bool accionable = false; 
	bool numb = false; 
	bool resetable = false; 

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