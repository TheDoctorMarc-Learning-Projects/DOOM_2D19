#include "UiItem.h"
#include "j1Gui.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Input.h"
#include "j1Window.h"
#include "Brofiler/Brofiler.h"
 

UiItem::UiItem(const iPoint & pos, UiItem * const parent)
{
	hitBox.x = pos.x;
	hitBox.y = pos.y;
	if (parent != nullptr)
		this->parent = parent;



	SDL_ShowCursor(SDL_DISABLE);
}

UiItem::UiItem(const iPoint& pos, std::string& name, UiItem* const parent)
{
	hitBox.x = pos.x;
	hitBox.y = pos.y;
	this->name = name;
	if (parent != nullptr)
		this->parent = parent;

}


UiItem::UiItem(UiItem * const parent)
{

	if (parent != nullptr)
		this->parent = parent;
}

UiItem::UiItem(std::string name)
{
	this->name = name; 
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


/*                                        // TODO: have a nice cursor when on menus. Disable it ingame ( the original skull cursor?) 
void UiItem::Draw_Cursor(float dt) {
	int x, y;
	x = y = 0;
	App->input->GetMousePosition(x, y);
	x *= App->win->GetScale();
	y *= App->win->GetScale();


	SDL_Rect section = { 252,638,25,32 };       // TODO: replace this cursor coordinates with the new one
	App->render->BlitGui(App->gui->GetAtlas(), x, y, &section, 0.0F);



}*/