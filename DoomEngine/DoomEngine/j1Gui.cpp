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
#include "j1Fonts.h"

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
	spriteScale = conf.child("spriteScale").attribute("value").as_float();

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

		std::string textureName = uiNode.child("textureMapName").attribute("value").as_string();
		SDL_Texture* nexTexture = nullptr; 
		if (textureName != "")
			nexTexture = App->map->entityTextureMap.at(textureName); 

	    App->gui->AddImage(position, &section, name, NULL, false, nexTexture, 0.0F);
	}


	// labels
	for (pugi::xml_node uiNode = menuNode.child("labels").child("label"); uiNode; uiNode = uiNode.next_sibling("label"))
	{
		std::string name = uiNode.attribute("name").as_string();
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		std::string text = uiNode.child("text").attribute("value").as_string();
		std::string font = uiNode.child("font").attribute("value").as_string();
		SDL_Color color = { uiNode.child("color").attribute("R").as_uint(),uiNode.child("color").attribute("G").as_uint(),uiNode.child("color").attribute("B").as_uint(),uiNode.child("color").attribute("A").as_uint() };
		const char* path = uiNode.child("path").attribute("p").as_string();
		uint size = uiNode.child("size").attribute("s").as_int();
		float scaleFactor = uiNode.child("scaleFactor").attribute("value").as_float(); 

		App->gui->AddLabel(name, text.data(), color, App->font->Load(path, size), position, NULL, scaleFactor);

	}

	// face
	pugi::xml_node uiNode = menuNode.child("face"); 
		std::string name = uiNode.attribute("name").as_string();
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		App->gui->AddFace(position, name, NULL, 0.0F);
	


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

UiItem_Label * j1Gui::AddLabel(std::string name, std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem * const parent, float SpriteScale)
{
	UiItem* newUIItem = nullptr;
	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Label(name, text, color, font, position, currentCanvas, SpriteScale);
	else
		newUIItem = DBG_NEW UiItem_Label(name, text, color, font, position, parent, SpriteScale); 

	listOfItems.push_back(newUIItem);
	return (UiItem_Label*)newUIItem;
}

UiItem_Image* j1Gui::AddImage(iPoint position, const SDL_Rect* section, std::string name, UiItem* const parent, bool isTabbable, SDL_Texture* specialTex, float spriteScale)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Image(position, section, name, currentCanvas, isTabbable, specialTex, spriteScale);
	else
		newUIItem = DBG_NEW UiItem_Image(position, section, name, parent, isTabbable, specialTex, spriteScale);

	listOfItems.push_back(newUIItem);

	return (UiItem_Image*)newUIItem;
}

 


UiItem_Bar* j1Gui::AddBar(iPoint position, std::string name, const SDL_Rect * section, const SDL_Rect * thumb_section, const SDL_Rect * image_idle, const SDL_Rect * image_hover, UiItem * const parent)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Bar(position, name, section, thumb_section, image_idle, image_hover, currentCanvas);
	else
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



UiItem_Face* j1Gui::AddFace(iPoint position, std::string name, UiItem* const parent, float spriteScale)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Face(position, name, currentCanvas, spriteScale);
	else
		newUIItem = DBG_NEW UiItem_Face(position, name, parent, spriteScale);

	listOfItems.push_back(newUIItem);

	return (UiItem_Face*)newUIItem;
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

UiItem* j1Gui::GetItemByName(std::string name, UiItem* parent) const  // searches item only in the current canvas childhood 
{
	if (parent == nullptr)   // if parent is not defined, start searching from canvas 
		parent = currentCanvas;

	for (const auto& item : parent->children)
	{
		if (item->name == name)  // if found, return 
			return item;
		else
			if (item->children.size() > 0)   // do a recursive call with the child's children
			{
				UiItem* success = GetItemByName(name, item);

				if (success != nullptr)
					return success;
			}
	}

	return nullptr;
	
}


void j1Gui::UpDateInGameUISlot(std::string name, float newValue, SDL_Rect newSection)   // needed for ammo, health, armor
{
	if (newValue < 0.0f)
		newValue = 0.0f; 

	UiItem* item = GetItemByName(name); 

	if (item == nullptr)
		return; 

	if (item->guiType == GUI_TYPES::LABEL)
	{
		float lastWidth = item->section.w * item->scaleFactor;
		dynamic_cast<UiItem_Label*>(item)->ChangeTextureIdle(std::to_string((int)newValue), NULL, NULL);
		float newWidth = item->section.w * item->scaleFactor;

		item->hitBox.x -= (int)((newWidth - lastWidth) * .5F); 
	}
	else if (item->guiType == GUI_TYPES::IMAGE)    // needed for the weapon image 
	{
		float lastWidth = item->section.w * item->scaleFactor;
		item->section = newSection; 
		float newWidth = item->section.w * item->scaleFactor;

		item->hitBox.x -= (int)((newWidth - lastWidth) * .5F);
	}


}
 
void j1Gui::UpdateDeathTimer(int value)
{
	if (App->entityFactory->IsPlayerAlive() == false)
		return; 

	if (App->entityFactory->player->godMode == true)
		return; 

	UiItem_Label* myLabel = (UiItem_Label*)App->gui->GetItemByName("deathTimerCounter");

	int time = std::stoi(myLabel->text);
	time += value;

	if (time >= 0)
	{
		UpDateInGameUISlot("deathTimerCounter", time);
		if(time == 0)
			App->entityFactory->player->SetDyingState();
	}


}