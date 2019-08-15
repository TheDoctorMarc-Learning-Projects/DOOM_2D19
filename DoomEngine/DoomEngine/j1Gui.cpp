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
#include "j1FadeToBlack.h"
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
	 { "ExitGame", &ExitGame},
	 { "SetDifficulty", &SetDifficulty},
	 { "SetVolume", &SetVolume},
	};

	// TODO: keep updating this function map

	defaultInGameStats = {
		{ "healthLabel", 500},
		{ "armorLabel", 0},
		{ "ammoLabel", 0},
		{ "oldCollectibleLabel", 0},
		{ "newCollectibleLabel", 0},
		{ "deathTimerCounter", 40},
	}; 
}


j1Gui::~j1Gui()
{
	functionsMap.clear(); 
	defaultInGameStats.clear(); 
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

	// load the main menu GUI 
	LoadGuiDefined(sceneTypeGUI::MAINMENU); 
	App->scene->SetSceneState(SceneState::MAINMENU); 
	App->audio->PlayMusic(App->scene->sceneMusics.at(SceneState::MAINMENU), -1);


	return true;
}


bool j1Gui::Update(float dt)
{
	BROFILER_CATEGORY("Update_Gui.cpp", Profiler::Color::Coral)

	if (App->scene->GetCurrentSceneTypeGui() == sceneTypeGUI::LEVEL)
		return true; 

		this->dt = dt;
	iPoint mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);
	uint mouseState = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);


	// for the moment, this won't update the grandsons (not needed) 

	for (auto& item : currentCanvas->GetChildrenRecursive())
	{
		if (item->enable == false || item->numb == true)
			continue; 

		if (App->entityFactory->isPointInsideRect(&mousePos, &item->hitBox) == true)   // MOUSE INSIDE HITBOX
		{


			if (item->state == DRAG)
				item->OnDrag(mousePos);

			if (item->state != CLICK && mouseState == KEY_DOWN)
			{
				
				if (selectedItem == nullptr && item->accionable == true)
				{
					item->state = CLICK;
					selectedItem = item;
					item->OnClickDown(mousePos);
				}
					
			}

			if (item->state != DRAG && mouseState == KEY_REPEAT)
			{
				if (selectedItem == item)
				{
					if (item->draggable == true)
					{
						item->state = DRAG;
						item->OnDrag(mousePos);
					}
						
				}
				
			}


			if ((item->state == CLICK || item->state == DRAG) && mouseState == KEY_UP)
			{
				if (item->state == DRAG && item->focusable == false)
					item->SetOriginPos(); 

				item->state = HOVER;
				selectedItem = nullptr;
				item->OnClickUp();
			
			}

			else if (item->state == IDLE)
			{
				item->OnHover(); 
				item->state = HOVER;
			}
				


		}
		else if (item->state != IDLE)  // MOUSE OUTSIDE HITBOX
		{
			if (item->focusable == true)
			{
				if (item->state == DRAG)          // keep it on drag state until key up
				{
					if(mouseState != KEY_UP)
						item->OnDrag(mousePos);
					else
					{
						item->state = IDLE;
						item->OnClickUp();
					}

				}
			}
			else
			{
				if (item->state == DRAG)
					item->SetOriginPos();

				item->state = IDLE;
				selectedItem = nullptr;
				item->OnHoverExit();
			}
		
		}
			
	 

	}

	lastMousePos = mousePos; 

	return true;
}




bool j1Gui::PostUpdate()
{

	BROFILER_CATEGORY("UI PostUpdate", Profiler::Color::Yellow);
 
 
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)    // TODO: change with proper UI debug key
		debug = !debug;

	if(currentCanvas != nullptr)
		if(App->fade->GetCurrentStep() == fade_step::none)  // do not draw gui until fade is complete
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

	selectedItem = nullptr; 

	return true;
}


void ExitGame(UiItem* callback)
{
	App->buttonClosesApp = true; 
}

void SetDifficulty(UiItem* callback)
{
	int diffLevel = 1; 

	for (const auto& item : App->gui->GetCurrentCanvas()->children)   // search which difficulty level the button is related to according to the Y pos
		if(item->functionName == "SetDifficulty")
			if (item->hitBox.y < callback->hitBox.y)
				diffLevel++;

		
	App->entityFactory->SetDifficultyMultiplier(diffLevel); 
}


void SetVolume(UiItem* callback)
{
	// synchro with bars: the top one is mus and fx's are below
	int pos = 1; 

	for (const auto& child : App->gui->GetCurrentCanvas()->GetChildrenRecursive())
		if (child->parent && child->parent->guiType == BAR)
			if (child->hitBox.y < callback->hitBox.y)
				pos++; 

	float volume = dynamic_cast<UiItem_Bar*>(callback)->getThumbTravelFactor(); 

	if (pos == 1)
		App->audio->SetVolume(volume);
	else
		App->audio->SetFxVolume(volume); 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - create a canvas from a button action
void LoadGui(UiItem* callback)   
{
	bool found = false; 
	for (auto& canvas : App->gui->GetCanvases())
	{
		if (canvas.second->myScene == callback->targetSceneGui)     // if there exists a current canvas with the scene
		{
			found = true; 
			App->gui->ChangeCurrentCanvas(canvas.second, true);   // just change the current canvas
		}

	}

	// if not found, create it: 
	if(found == false) 
		App->gui->ChangeCurrentCanvas(DBG_NEW UiItem(App->scene->sceneGuiXMLIndexes.at(callback->targetSceneGui), callback->targetSceneGui), false);


	// finally, call scene swap (it will call fade, and then fade will call create scene)
	App->scene->LoadScene(callback->targetScene, false); 


}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - create a canvas from external action, eg collider win, ingame ESC...
void j1Gui::LoadGuiDefined(sceneTypeGUI targetScene)   
{

	for (auto& canvas : App->gui->GetCanvases())
	{
		if (canvas.second->myScene == targetScene)     // if there exists a current canvas with the scene
		{
			App->gui->ChangeCurrentCanvas(canvas.second, true);   // just change the current canvas
			return;
		}

	}

	// if not found, create it: 
	App->gui->ChangeCurrentCanvas(DBG_NEW UiItem(App->scene->sceneGuiXMLIndexes.at(targetScene), targetScene), false);

}


void j1Gui::ChangeCurrentCanvas(UiItem* newCanvas, bool exists)
{
	// A) push the new canvas in the map and set to current one    

	if (exists == false)                             
	{
		if(currentCanvas)
			currentCanvas->EnableChildren(false);
	
		canvases.insert(std::make_pair(newCanvas->myScene, newCanvas));
		currentCanvas = newCanvas;
		currentCanvas->EnableChildren(true);

		// finally load the GUI with the canvas name 
		LoadXMLGUI((pugi::xml_node&)App->scene->sceneNode.child(currentCanvas->name.data()));   
	}
		  
	// B) if it already exists in the map, just assign it

	else 
	{
		if (currentCanvas)
			currentCanvas->EnableChildren(false);

		if (currentCanvas->myScene == sceneTypeGUI::LEVEL)  // TODO: do not reset gui if next scene state is ingame settings
			ResetInGameUI();

		currentCanvas = newCanvas; 
		currentCanvas->EnableChildren(true);

	}
	
	if(selectedItem)
		selectedItem = nullptr;
	

	// hide the cursor if in game 
/*	if(App->scene->GetCurrentSceneType() == sceneType::LEVEL)
		SDL_ShowCursor(SDL_DISABLE);
	else
		SDL_ShowCursor(SDL_ENABLE);*/
	
}

void j1Gui::LoadXMLGUI(pugi::xml_node& menuNode)
{
	// images 
	for (auto uiNode = menuNode.child("images").child("image"); uiNode; uiNode = uiNode.next_sibling("image"))
	{
		std::string name = uiNode.attribute("name").as_string();
		SDL_Rect section = { uiNode.child("section").attribute("x").as_int(), uiNode.child("section").attribute("y").as_int(), uiNode.child("section").attribute("w").as_int(), uiNode.child("section").attribute("h").as_int() };
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };

		std::string textureName = uiNode.child("textureMapName").attribute("value").as_string();
		SDL_Texture* newTexture = nullptr; 
		if (textureName != "")
			newTexture = App->map->entityTextureMap.at(textureName); 
		float scaleFactor = uiNode.child("scaleFactor").attribute("value").as_float();

	    App->gui->AddImage(position, &section, name, NULL, false, newTexture, scaleFactor, textureName);
	}


	// labels
	for (auto uiNode = menuNode.child("labels").child("label"); uiNode; uiNode = uiNode.next_sibling("label"))
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

	// buttons
	for (auto uiNode = menuNode.child("buttons").child("button"); uiNode; uiNode = uiNode.next_sibling("button"))
	{
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		std::string name = uiNode.attribute("name").as_string();
		std::string functionName = uiNode.child("functionName").attribute("value").as_string();
		std::string text = uiNode.child("text").attribute("value").as_string();
		std::string font = uiNode.child("font").attribute("value").as_string();
		SDL_Color color = { uiNode.child("color").attribute("R").as_uint(),uiNode.child("color").attribute("G").as_uint(),uiNode.child("color").attribute("B").as_uint(),uiNode.child("color").attribute("A").as_uint() };
		const char* fontPath = uiNode.child("path").attribute("p").as_string();
		uint fontSize = uiNode.child("size").attribute("s").as_int();
		float scaleFactor = uiNode.child("scaleFactor").attribute("value").as_float();
		int targetScene = uiNode.child("targetScene").attribute("value").as_int();

		App->gui->AddButton(position, functionName, name, text.data(), color, App->font->Load(fontPath, fontSize), NULL, scaleFactor, (SceneState)targetScene);

	}
	// sliders
	for (auto uiNode = menuNode.child("sliders").child("slider"); uiNode; uiNode = uiNode.next_sibling("slider"))
	{
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		std::string name = uiNode.attribute("name").as_string();
		std::string functionName = uiNode.child("functionName").attribute("value").as_string();
		SDL_Rect section = { uiNode.child("section").attribute("x").as_int(), uiNode.child("section").attribute("y").as_int(), uiNode.child("section").attribute("w").as_int(), uiNode.child("section").attribute("h").as_int() };
		SDL_Rect hoverSection = { uiNode.child("hoverSection").attribute("x").as_int(), uiNode.child("hoverSection").attribute("y").as_int(), uiNode.child("hoverSection").attribute("w").as_int(), uiNode.child("hoverSection").attribute("h").as_int() };
		SDL_Rect thumbSection = { uiNode.child("thumbSection").attribute("x").as_int(), uiNode.child("thumbSection").attribute("y").as_int(), uiNode.child("thumbSection").attribute("w").as_int(), uiNode.child("thumbSection").attribute("h").as_int() };
		float scaleFactor = uiNode.child("scaleFactor").attribute("value").as_float();
		iPoint thumbOffset = { uiNode.child("thumbOffset").attribute("x").as_int(), uiNode.child("thumbOffset").attribute("y").as_int() };

		App->gui->AddBar(position, thumbOffset, name, functionName, &section, &thumbSection, &hoverSection, NULL, scaleFactor);

	}

	// face
	std::string menuName = menuNode.name(); 
	if (menuName == "InGameUI")
	{
		auto uiNode = menuNode.child("face");
		std::string name = uiNode.attribute("name").as_string();
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		App->gui->AddFace(position, name, NULL, 0.0F);

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

UiItem_Image* j1Gui::AddImage(iPoint position, const SDL_Rect* section, std::string name, UiItem* const parent, bool isTabbable, SDL_Texture* specialTex, float spriteScale, std::string newTextureName)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Image(position, section, name, currentCanvas, isTabbable, specialTex, spriteScale, newTextureName);
	else
		newUIItem = DBG_NEW UiItem_Image(position, section, name, parent, isTabbable, specialTex, spriteScale);

	listOfItems.push_back(newUIItem);

	return (UiItem_Image*)newUIItem;
}

 


UiItem_Bar* j1Gui::AddBar(iPoint position, iPoint thumbOffset, std::string name, std::string functionName, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* sectioHover,
	UiItem*const parent, float Spritescale)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Bar(position, thumbOffset, name, functionName, section, thumb_section, sectioHover, currentCanvas, Spritescale);
	else
		newUIItem = DBG_NEW UiItem_Bar(position, thumbOffset, name, functionName, section, thumb_section, sectioHover, parent, Spritescale);

	listOfItems.push_back(newUIItem);

	return (UiItem_Bar*)newUIItem;
}



UiItem_Button* j1Gui::AddButton(iPoint position, std::string functionName, std::string name, std::string text, SDL_Color color, TTF_Font * font, UiItem* const parent, float spriteScale, SceneState targetScene)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Button(position, functionName, name, text, color, font, currentCanvas, spriteScale, targetScene);
	else
		newUIItem = DBG_NEW UiItem_Button(position, functionName, name, text, color, font, parent, spriteScale, targetScene);

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


void j1Gui::ResetInGameUI()
{
	// reset the stats do default values
	for (auto& item : currentCanvas->children)
		if(item->name != "panel" && item->name != "oldCollectible" && item->name != "newCollectible" && item->name != "deathTimerLabel" && item->name != "face" && item->name != "weaponImage")
			UpDateInGameUISlot(item->name, defaultInGameStats.at(item->name));
		
	// The texture map in map cpp, where the loot texture was, has been created again after map swap, so update the items that get the texture from it
	for (auto& item : currentCanvas->children)
		if (item->guiType == GUI_TYPES::IMAGE)
			if(dynamic_cast<UiItem_Image*>(item)->specialTex != nullptr)
				dynamic_cast<UiItem_Image*>(item)->ReAssignSpecialTexture();
			

	// finally reset the face: 
	dynamic_cast<UiItem_Face*>(GetItemByName("face"))->SetCurrentAnim("idle");
}