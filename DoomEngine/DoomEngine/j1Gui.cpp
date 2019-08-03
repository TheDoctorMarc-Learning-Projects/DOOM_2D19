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
#include "j1Scene.h"

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

	// TODO: keep updating this function map

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

 

	// the first time just load the main menu GUI
	initializeGUI(); 

	return true;
}

void j1Gui::initializeGUI()   // set the state to Main Menu while creating a Main Menu canvas
{
	ChangeCurrentCanvas(sceneTypeGUI::INGAME, DBG_NEW UiItem(App->scene->sceneGuiXMLIndexes.at(sceneTypeGUI::INGAME)));  // TODO: change to main menu
}


bool j1Gui::Update(float dt)
{
//	DoLogicSelected(); // TODO: this must only consider current canvas children !! 


	return true;
}




bool j1Gui::PostUpdate()
{

	BROFILER_CATEGORY("UI PostUpdate", Profiler::Color::Yellow);
 
 
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)    // TODO: change with proper UI debug key
		debug = !debug;

	if(currentCanvas != nullptr)
		currentCanvas->DrawChildren();


	return true;
}

bool j1Gui::CleanUp()
{
	if (atlas != nullptr)
	{
		App->tex->UnLoad(atlas);
		atlas = nullptr;
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

	canvases.clear();

	
	return true;
}

void LoadGui(UiItem* callback)  
{
	if (callback->targetScene != sceneTypeGUI::NO_SCENE)
	{
		// CASE A) The menu GUI and canvas have not been created
		if (callback->hasExecutedFunction == false)    
			App->gui->ChangeCurrentCanvas(callback->targetScene, DBG_NEW UiItem(App->scene->sceneGuiXMLIndexes.at(callback->targetScene)));

		// CASE B) the menu GUI and canvas already exist 
		else
			App->gui->ChangeCurrentCanvas(callback->targetScene);   
		
	}
}

void j1Gui::ChangeCurrentCanvas(sceneTypeGUI targetScene, UiItem* newCanvas)
{
	if (newCanvas != nullptr)    // A) create a new canvas in the map and set to current one                               
	{
		canvases.insert(std::make_pair(targetScene, newCanvas));
		currentCanvas = newCanvas;

		// finally load the GUI with the canvas name 
		LoadXMLGUI((pugi::xml_node&)App->scene->sceneNode.child(currentCanvas->name.data()));   
	}
		   
	else
		currentCanvas = canvases.at(targetScene);  // B) find the canvas in the map         	
	 
}

void j1Gui::LoadXMLGUI(pugi::xml_node& menuNode)
{
	for (pugi::xml_node uiNode = menuNode.child("images").child("image"); uiNode; uiNode = uiNode.next_sibling("image"))
	{
		std::string name = uiNode.attribute("name").as_string();

		SDL_Rect section = { uiNode.child("section").attribute("x").as_int(), uiNode.child("section").attribute("y").as_int(), uiNode.child("section").attribute("w").as_int(), uiNode.child("section").attribute("h").as_int() };
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };

		App->gui->AddImage(position, &section, name, NULL, false);  // bug: an image is created as panel 
	}

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



UiItem_Button* j1Gui::AddButton(iPoint position, std::string function, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click, const SDL_Rect* hover, sceneTypeGUI targetScene)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Button(position, function, name, idle, currentCanvas, click, hover, targetScene);
	else
		newUIItem = DBG_NEW UiItem_Button(position, function, name, idle, parent, click, hover, targetScene);

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
 
 