#include "UiItem.h"
#include "j1Gui.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Window.h"
#include "Brofiler/Brofiler.h"
 

UiItem::UiItem(const iPoint pos, UiItem * const parent)
{
	originPos = pos; 
	hitBox.x = pos.x;
	hitBox.y = pos.y;


	// the parent
	AssignParentChild(parent);

}

UiItem::UiItem(const iPoint pos, std::string name, UiItem* const parent)
{
	originPos = pos;
	hitBox.x = pos.x;
	hitBox.y = pos.y;
	this->name = name;


	// the parent
	AssignParentChild(parent);

}


UiItem::UiItem(UiItem * const parent)
{

	// the parent
	AssignParentChild(parent);
}

UiItem::UiItem(std::string name, sceneTypeGUI myscene)
{
	this->name = name; 
	this->myScene = myscene; 

}



 

UiItem::~UiItem()
{
	//LOG("destroyed UI item somewhere");

}
 
void UiItem::DrawChildren()    // what about "to_delete"? is it needed for the gui ?
{
	if (children.size() > 0)
		for (const auto& child : children)
			if (child->enable == true && child->hide == false)
			{
				child->Draw();
				if (App->gui->debug == true)
					child->DebugDraw();


				if (child->children.size() > 0)
					child->DrawChildren(); 
			}		
	
}

 

void UiItem::DebugDraw()
{
	if (parent == nullptr)
		return; 
	if (name == "background")
		return; 
	if (guiType != IMAGE && guiType != LABEL)
		return; 

	SDL_Rect target = hitBox; 
	target.w *= scaleFactor; 
	target.h *= scaleFactor;

	if (target.w == 0 || target.h == 0)
	{
		target.w = section.w * scaleFactor;
		target.h = section.h * scaleFactor;
	}

	App->render->DrawQuad(target, 0, 0, 255, 50, true, true);
}


void UiItem::MoveWithMouse(iPoint mousePos)
{
	iPoint offset = iPoint(0, 0);
	if (state == CLICK)
		offset = offsetWithMouse = mousePos - iPoint(hitBox.x, hitBox.y);  // store offset in the offsetwithmouse header variable

	iPoint mouseTravel = mousePos - App->gui->GetLastMousePos(); 
	iPoint targetPos = iPoint(hitBox.x + mouseTravel.x - offset.x, hitBox.y + mouseTravel.y - offset.y);
	SetPos(targetPos);
	
	if (children.size() > 0)
		for (const auto& child : children)
			if (child->enable == true)
			{
				child->MoveWithMouse(mousePos);
			}
}
 
 