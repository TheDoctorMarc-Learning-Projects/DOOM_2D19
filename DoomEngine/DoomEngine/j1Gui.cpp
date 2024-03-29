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
	 { "ChangeGamePause", &ChangeGamePause },
	 { "ClippingScroll", &ClippingScroll },
	 { "Continue", &Continue },
	 { "OpenWeb", &OpenWeb }
	};

	defaultInGameStats = {
		{ "healthLabel", 500},
		{ "armorLabel", 0},
		{ "ammoLabel", 0},
		{ "oldCollectibleLabel", 0},
		{ "newCollectibleLabel", 0},
		{ "deathTimerCounter", 40},
		{ "LiveCounter", 3},
		{ "TimeCounter", 0},
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
	App->scene->SetSceneState(SceneState::MAINMENU);
	LoadGuiDefined(sceneTypeGUI::MAINMENU); 
	App->audio->PlayMusic(App->scene->sceneMusics.at(SceneState::MAINMENU), -1);


	return true;
}

bool j1Gui::PreUpdate()
{
	BROFILER_CATEGORY("GUI PreUpdate", Profiler::Color::Gainsboro);

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		if(currentCanvas->myScene == sceneTypeGUI::LEVEL)
			ChangeGamePause(nullptr);
	
	for (auto& item : currentCanvas->GetChildrenRecursive())
		if (item->enable == true)
			if (item->targPosData.toUpdatePos == true)
			{
				item->hitBox.x += item->targPosData.speed.x;
				item->hitBox.y += item->targPosData.speed.y;

				if (item->GetPos() == item->targPosData.targetPos)
					item->targPosData.toUpdatePos = false;
			}
	



	return true; 
}

bool j1Gui::Update(float dt)
{
	BROFILER_CATEGORY("GUI Update", Profiler::Color::Gainsboro);

	if (App->scene->GetCurrentSceneTypeGui() == sceneTypeGUI::LEVEL)
		return true; 

		this->dt = dt;
	iPoint mousePos;
	App->input->GetMousePosition(mousePos.x, mousePos.y);
	uint mouseState = App->input->GetMouseButtonDown(SDL_BUTTON_LEFT);


    // go through all current canvas family tree 
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

	BROFILER_CATEGORY("GUI PostUpdate", Profiler::Color::Gainsboro);
 
	if (App->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)    
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

	for (auto& item : listOfItems)    
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


void OpenWeb(UiItem* callback)
{ 
	ShellExecuteA(NULL, "open", "https://mdpdoctor7.wixsite.com/doomproject", NULL, NULL, SW_SHOWNORMAL); 
}

void Continue(UiItem* callback)
{
	App->scene->SaveLoadLogic(false, callback); 
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
	// 1) synchro with bars: the top one is mus and fx's are below
	int pos = 1; 

	for (const auto& child : App->gui->GetCurrentCanvas()->GetChildrenRecursive())
		if (child->parent && child->parent->guiType == BAR)
			if (child->hitBox.y < callback->hitBox.y)
				pos++; 

	
	float volume = dynamic_cast<UiItem_Bar*>(callback)->getThumbTravelFactor(); 
	

	// 2)  set the volume according to thumb: make a fx sound on top of the current music
    if (dynamic_cast<UiItem_Bar*>(callback)->thumb->state != IDLE && dynamic_cast<UiItem_Bar*>(callback)->thumb->state != HOVER)
	{

	   if (App->audio->isPlayingFx(callback->name) == false)
		   App->audio->PlayFx(callback->name, 0, true, 1.f, 0.f, 0.f);
	   else
		   App->audio->SetSpecificFxVolume(callback->name, volume);
	}
	else 	// 3)  when releasing the slider, stop the current fx, reset the mus and fxs to previous state and then assign the last volume 
	{
		App->audio->StopSpecificFx(callback->name);
		App->audio->ResetMusicAndFxVolumes();

		if (pos == 1)
			App->audio->SetVolume(volume);
		else
			App->audio->SetFxVolume(volume);
	}

}

// no need to do anything generic here, will be done just once 
// (could have smth defined in xml to bond the button and the related image)
void ClippingScroll(UiItem* callback)
{
	UiItem* targetItem = App->gui->GetCanvasItemByName("license");

	if (callback->name == "creditScrollButton")
	{
		if (targetItem->targPosData.toUpdatePos == true)
			return; 

		targetItem->targPosData.targetPos = iPoint(targetItem->GetPos().x, -670);
		targetItem->targPosData.speed = iPoint(0, -1);
		targetItem->targPosData.toUpdatePos = true;
	}
	else if (callback->name == "creditScrollResetButton")
	{
		targetItem->hitBox.y = targetItem->originPos.y; 
		targetItem->targPosData.toUpdatePos = false;
	}
	
 
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - create a canvas from a button action
void LoadGui(UiItem* callback)   
{
	if (App->pause == true)  // Extra: when coming from InGame Settings, unPause and reset lives 
	{
		App->pause = false;
		App->entityFactory->playerLives = 3;
		App->gui->UpDateInGameUISlot("LiveCounter", 3);
	}
		
	
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
	if (found == false)
		App->gui->ChangeCurrentCanvas(DBG_NEW UiItem(App->scene->sceneGuiXMLIndexes.at(callback->targetSceneGui), callback->targetSceneGui), false);


	// finally, call scene swap (it will call fade, and then fade will call create scene)
	App->scene->LoadScene(callback->targetScene, false);
}

void ChangeGamePause(UiItem* callback)
{

	App->pause = !App->pause; 

	// coming from InGame Settings --> get the scene before the change (a level) 
	SceneState target = App->scene->GetPreviousSceneState(); 
	sceneTypeGUI targetGUI = App->scene->convertSceneTypeToGui(target);


	// coming from ingame ESC --> go to InGame Settings, set previous state as level
	if (callback == nullptr)
	{
		App->scene->SetPreviousSceneState(App->scene->GetCurrentSceneState());
		targetGUI = sceneTypeGUI::IN_GAME_SETTINGS;
		target = SceneState::IN_GAME_SETTINGS;
		App->audio->PauseMusic();
	}
	else
		App->audio->ResumeMusic(); 

	App->gui->LoadGuiDefined(targetGUI); 
	App->scene->SetSceneState(target); 
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

		// reset ingame ui if needed, or pause the app 
		if ((currentCanvas->myScene == sceneTypeGUI::LEVEL && newCanvas->myScene == sceneTypeGUI::IN_GAME_SETTINGS)
			|| (currentCanvas->myScene == sceneTypeGUI::IN_GAME_SETTINGS && newCanvas->myScene == sceneTypeGUI::LEVEL))
			LOG("Do not reset InGame UI!"); 
		else
			ResetInGameUI();

		currentCanvas = newCanvas; 
		currentCanvas->EnableChildren(true);

	}
	
	if(selectedItem)
		selectedItem = nullptr;
	

	// hide the cursor if in game 
	if(newCanvas->myScene == sceneTypeGUI::LEVEL)
		SDL_ShowCursor(SDL_DISABLE);
	else
		SDL_ShowCursor(SDL_ENABLE);
	
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

	    App->gui->AddImage(position, &section, name, NULL, newTexture, scaleFactor, textureName);
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
		bool resetable = uiNode.child("resetable").attribute("value").as_bool();
		App->gui->AddLabel(name, text.data(), color, App->font->Load(path, size), position, NULL, scaleFactor, resetable);

	}

	// buttons
	for (auto uiNode = menuNode.child("buttons").child("button"); uiNode; uiNode = uiNode.next_sibling("button"))
	{
		iPoint position = { uiNode.child("position").attribute("x").as_int(), uiNode.child("position").attribute("y").as_int() };
		std::string name = uiNode.attribute("name").as_string();
		std::string functionName = uiNode.child("functionName").attribute("value").as_string();
		std::string text = uiNode.child("text").attribute("value").as_string();
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

		std::string text = uiNode.child("text").attribute("value").as_string();
		std::string valueText = uiNode.child("valueText").attribute("value").as_string();
		SDL_Color color = { uiNode.child("color").attribute("R").as_uint(),uiNode.child("color").attribute("G").as_uint(),uiNode.child("color").attribute("B").as_uint(),uiNode.child("color").attribute("A").as_uint() };
		SDL_Color color2 = { uiNode.child("color2").attribute("R").as_uint(),uiNode.child("color2").attribute("G").as_uint(),uiNode.child("color2").attribute("B").as_uint(),uiNode.child("color2").attribute("A").as_uint() };
		const char* fontPath = uiNode.child("path").attribute("p").as_string();
		uint fontSize = uiNode.child("size").attribute("s").as_int();

		App->gui->AddBar(position, thumbOffset, name, functionName, &section, &thumbSection, &hoverSection, NULL, scaleFactor, 
			text, valueText, color, color2, App->font->Load(fontPath, fontSize));

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


UiItem_Label * j1Gui::AddLabel(std::string name, std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem * const parent, float SpriteScale, bool resetable)
{
	UiItem* newUIItem = nullptr;
	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Label(name, text, color, font, position, currentCanvas, SpriteScale, resetable);
	else
		newUIItem = DBG_NEW UiItem_Label(name, text, color, font, position, parent, SpriteScale, resetable);

	listOfItems.push_back(newUIItem);
	return (UiItem_Label*)newUIItem;
}

UiItem_Image* j1Gui::AddImage(iPoint position, const SDL_Rect* section, std::string name, UiItem* const parent, SDL_Texture* specialTex, float spriteScale, std::string newTextureName)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Image(position, section, name, currentCanvas, specialTex, spriteScale, newTextureName);
	else
		newUIItem = DBG_NEW UiItem_Image(position, section, name, parent, specialTex, spriteScale);

	listOfItems.push_back(newUIItem);

	return (UiItem_Image*)newUIItem;
}

 


UiItem_Bar* j1Gui::AddBar(iPoint position, iPoint thumbOffset, std::string name, std::string functionName, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* sectioHover,
	UiItem*const parent, float Spritescale, std::string labelText, std::string valueLabelText, SDL_Color textColor, SDL_Color valueColor, TTF_Font * font)
{
	UiItem* newUIItem = nullptr;

	if (parent == NULL)
		newUIItem = DBG_NEW UiItem_Bar(position, thumbOffset, name, functionName, section, thumb_section, sectioHover, currentCanvas, Spritescale , labelText, valueLabelText, textColor, valueColor, font);
	else
		newUIItem = DBG_NEW UiItem_Bar(position, thumbOffset, name, functionName, section, thumb_section, sectioHover, parent, Spritescale, labelText, valueLabelText, textColor, valueColor, font);

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


UiItem* j1Gui::GetCanvasItemByName(std::string name, UiItem* parent) const  // searches item only in the current canvas childhood 
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
				UiItem* success = GetCanvasItemByName(name, item);

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


	UiItem_Label* totalTimeLabel = (UiItem_Label*)GetCanvasItemByName("TimeCounter");
	if (totalTimeLabel == nullptr)
		return; 

	if (currentCanvas->myScene == sceneTypeGUI::LEVEL)
		UpDateInGameUISlot("TimeCounter", std::stoi(totalTimeLabel->text) + 1);


	if (App->entityFactory->player->godMode == true)
		return; 

	UiItem_Label* countdownLabel = (UiItem_Label*)GetCanvasItemByName("deathTimerCounter");
	if (countdownLabel == nullptr)
		return;

	int time = std::stoi(countdownLabel->text);
	time += value;

	if (time >= 0)
	{
		UpDateInGameUISlot("deathTimerCounter", time);
		if (time == 0)
			App->entityFactory->PlayerDeathLogic(); 
			
	}

	

}


void j1Gui::ResetInGameUI()
{
 
	// reset the stats do default values
	for (auto& item : listOfItems)
		if (item->resetable == true)
		{
			// do not reset the player lives when level to level (dying)
			if (item->name == "LiveCounter")
			{
				if (App->scene->GetCurrentSceneTypeGui() != sceneTypeGUI::LEVEL)
					UpDateInGameUISlot(item->name, defaultInGameStats.at(item->name));
			}
			else
				UpDateInGameUISlot(item->name, defaultInGameStats.at(item->name));
		}
			
		
	// The texture map in map cpp, where the loot texture was, has been created again after map swap, so update the items that get the texture from it
	for (auto& item : listOfItems)
		if (item->guiType == GUI_TYPES::IMAGE)
			if(dynamic_cast<UiItem_Image*>(item)->specialTex != nullptr)
				dynamic_cast<UiItem_Image*>(item)->ReAssignSpecialTexture();

	// finally reset the face 
	// !!! (do not call get from canvas, because you can be in ingame settings and the face belongs to ingame) 
	dynamic_cast<UiItem_Face*>(GetItemByName("face"))->SetCurrentAnim("idle");
}

bool j1Gui::Load(pugi::xml_node& node)
{
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("TimeCounter"))->name, (float)std::stoi(node.child("total_time").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("LiveCounter"))->name, (float)std::stoi(node.child("lives").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("deathTimerCounter"))->name, (float)std::stoi(node.child("death_time").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("ammoLabel"))->name, (float)std::stoi(node.child("ammo").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("healthLabel"))->name, (float)std::stoi(node.child("health").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("armorLabel"))->name, (float)std::stoi(node.child("armor").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("newCollectibleLabel"))->name, (float)std::stoi(node.child("new_collectible_count").attribute("value").as_string()));
	UpDateInGameUISlot(dynamic_cast<UiItem_Label*>(GetItemByName("oldCollectibleLabel"))->name, (float)std::stoi(node.child("old_collectible_count").attribute("value").as_string()));

	// update player real life count
	App->entityFactory->playerLives = (uint)(int)(float)std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("LiveCounter"))->text);

	// the face 
	dynamic_cast<UiItem_Face*>(GetItemByName("face"))->Load((pugi::xml_node&)node.child("face"));


	return true; 
}

bool j1Gui::Save(pugi::xml_node& node) const
{
	// Labels ---> just a value 
	node.append_child("total_time").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("TimeCounter"))->text); 
	node.append_child("lives").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("LiveCounter"))->text);
	node.append_child("death_time").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("deathTimerCounter"))->text);
	node.append_child("ammo").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("ammoLabel"))->text);
	node.append_child("health").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("healthLabel"))->text);
	node.append_child("armor").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("armorLabel"))->text);
	node.append_child("new_collectible_count").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("newCollectibleLabel"))->text);
	node.append_child("old_collectible_count").append_attribute("value") = std::stoi(dynamic_cast<UiItem_Label*>(GetItemByName("oldCollectibleLabel"))->text);

	// the face 
	dynamic_cast<UiItem_Face*>(GetItemByName("face"))->Save((pugi::xml_node&)node.append_child("face")); 

	return true; 
}