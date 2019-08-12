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
	App->render->DrawQuad(hitBox, 0, 0, 255, 255, true, true); 
}

 
 