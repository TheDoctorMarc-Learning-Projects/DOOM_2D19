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

	this->function = function;

	SDL_ShowCursor(SDL_DISABLE);
}

UiItem::UiItem(const iPoint& pos, std::string& name, UiItem* const parent)
{
	hitBox.x = pos.x;
	hitBox.y = pos.y;
	this->name = name;
	if (parent != nullptr)
		this->parent = parent;

	this->function = function;

}


UiItem::UiItem(UiItem * const parent)
{

	if (parent != nullptr)
		this->parent = parent;

	this->function = function;
}



UiItem::UiItem(const iPoint& pos, std::string& function, std::string& name, UiItem* const parent) : parent(parent)
{
	hitBox.x = pos.x;
	hitBox.y = pos.y;
	if (parent != nullptr)
		this->parent = parent;

	this->function = function;

	this->name = name;
}

//UiItem::UiItem(SDL_Rect hitBox, UiItem * const parent, p2Point<int> pivot) :pivot(pivot), parent(parent)
//{
//
//}

UiItem::~UiItem()
{
	//LOG("destroyed UI item somewhere");

}
 




/*                                        // TODO: have a nice cursor when on menus. Disable it ingame. 
void UiItem::Draw_Cursor(float dt) {
	int x, y;
	x = y = 0;
	App->input->GetMousePosition(x, y);
	x *= App->win->GetScale();
	y *= App->win->GetScale();


	SDL_Rect section = { 252,638,25,32 };       // TODO: replace this cursor coordinates with the new one
	App->render->BlitGui(App->gui->GetAtlas(), x, y, &section, 0.0F);



}*/