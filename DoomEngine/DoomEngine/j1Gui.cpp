#include "j1Gui.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Audio.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1EntityFactory.h"
#include "p2Log.h"
#include "Brofiler/Brofiler.h"

j1Gui::j1Gui() : j1Module()
{
	name.assign("gui");

	FillFunctionsMap(); 
}

void j1Gui::FillFunctionsMap()
{
	functionsMap = {
	 { "LoadGUI", &LoadGui},
	};

}


j1Gui::~j1Gui()
{
	functionsMap.clear(); 
}

bool j1Gui::Awake(pugi::xml_node& conf)
{
	bool ret = true;
	atlas_file_name = conf.child("atlas").attribute("file").as_string();
	return true;
}

bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.data());
	return true;
}

bool j1Gui::Update(float dt)
{
//	DoLogicSelected();


	return true;
}




bool j1Gui::PostUpdate()
{

	BROFILER_CATEGORY("UI PostUpdate", Profiler::Color::Yellow);
 
 
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)    // TODO: change with proper UI debug key
		debug = !debug;

	currentCanvas->DrawChildren();

	return true;
}

bool j1Gui::CleanUp()
{
	if (App->cleaningUp == true)   // only unload atlas when closing the App. Do not do it when switching the current UI
	{
		if (atlas != nullptr)
		{
			App->tex->UnLoad(atlas);
			atlas = nullptr;
		}

	}

 
	for (auto& item : listOfItems)   // clean the UI
	{
		if (item != nullptr)
		{
			item->CleanUp(); 
			delete item;
			item = nullptr;
		}
	}
	listOfItems.clear();


	return true;
}

void j1Gui::destroyElement(UiItem * elem)   // TODO: delete children first :/ This ain't gonna work easily 
{

	for (auto item = listOfItems.begin(); item != listOfItems.end(); ++item)
	{
		if (elem != nullptr && (*item) == elem)
		{
			(*item)->CleanUp();
			delete (*item);
			(*item) = nullptr;
			item = listOfItems.erase(item);
			break;

		}

	}

}

UiItem_Label * j1Gui::AddLabel(std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem * const parent)
{
	UiItem* newUIItem = nullptr;
	newUIItem = DBG_NEW UiItem_Label(text, color, font, position, parent);
	listOfItems.push_back(newUIItem);
	return (UiItem_Label*)newUIItem;
}

UiItem_Image* j1Gui::AddImage(iPoint position, const SDL_Rect* section, std::string& name, UiItem* const parent, bool isTabbable)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Image(position, section, name, currentCanvas, isTabbable);
	else
		newUIItem = DBG_NEW UiItem_Image(position, section, name, parent, isTabbable);

	listOfItems.push_back(newUIItem);

	return (UiItem_Image*)newUIItem;
}

 


UiItem_Bar* j1Gui::AddBar(iPoint position, std::string name, const SDL_Rect * section, const SDL_Rect * thumb_section, const SDL_Rect * image_idle, const SDL_Rect * image_hover, UiItem * const parent)
{
	UiItem* newUIItem = nullptr;

	newUIItem = DBG_NEW UiItem_Bar(position, name, section, thumb_section, image_idle, image_hover, parent);

	listOfItems.push_back(newUIItem);

	return (UiItem_Bar*)newUIItem;
}



UiItem_Button* j1Gui::AddButton(iPoint position, std::string function, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click, const SDL_Rect* hover)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Button(position, function, name, idle, currentCanvas, click, hover);
	else
		newUIItem = DBG_NEW UiItem_Button(position, function, name, idle, parent, click, hover);

	listOfItems.push_back(newUIItem);

	UiItem_Button * button = (UiItem_Button*)newUIItem;
 

	return (UiItem_Button*)newUIItem;
}


UiItem_Checkbox * j1Gui::AddCheckbox(iPoint position, std::string function, std::string name, const SDL_Rect * idle, UiItem * const parent, const SDL_Rect * click, const SDL_Rect * hover, const SDL_Rect * tick_section)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Checkbox(position, function, name, idle, currentCanvas, click, hover, tick_section);
	else
		newUIItem = DBG_NEW UiItem_Checkbox(position, function, name, idle, parent, click, hover, tick_section);

	listOfItems.push_back(newUIItem);

	return (UiItem_Checkbox*)newUIItem;
}


UiItem* j1Gui::AddEmptyElement(iPoint pos, UiItem * const parent)
{
	UiItem* newUIItem = nullptr;
	if (parent == NULL)
		newUIItem = DBG_NEW UiItem(pos, currentCanvas);
	else
		newUIItem = DBG_NEW UiItem(pos, parent);

	listOfItems.push_back(newUIItem);
	return newUIItem;
}
 
 