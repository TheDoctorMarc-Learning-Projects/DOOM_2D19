#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Scene.h"
#include "j1PathFinding.h"
#include "j1Fonts.h"
#include "j1Particles.h"
#include "j1Collision.h"        
#include "j1EntityFactory.h"
#include "j1BloodManager.h"
#include "j1FadeToBlack.h"
#include "j1Gui.h"
                                    
#include "Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
	state = SceneState::LEVEL1;



	sceneGuiXMLIndexes =
	{
		 {sceneTypeGUI::LEVEL, "InGameUI"},
		 {sceneTypeGUI::MAINMENU, "MainMenuUI"},
		 {sceneTypeGUI::DIFFICULTY_SELECTOR, "DifficultySelectorUI"},
		 {sceneTypeGUI::SETTINGS, "SettingsUI"},
		 {sceneTypeGUI::IN_GAME_SETTINGS, "InGameSettingsUI"},
	     {sceneTypeGUI::CREDITS, "CreditsUI"},
	};

	// TODO: keep updating this scene-gui index map

	sceneMusics =
	{
		 {SceneState::LEVEL1, "sound/music/Arch Enemy First Day In Hell.ogg"},
		 {SceneState::LEVEL2, "sound/music/soil-halo.ogg"},
		 {SceneState::MAINMENU, "sound/music/Manimal - Black Plague.ogg"},
		 {SceneState::DIFFICULTY_SELECTOR, "sound/music/Slipknot - Duality.ogg"},
		 {SceneState::SETTINGS, "sound/music/Disturbed - Stricken.ogg"},
		 {SceneState::IN_GAME_SETTINGS, "sound/music/Disturbed - Stricken.ogg"},
		 {SceneState::CREDITS, "sound/music/Arch Enemy - Set Flame to the Night.ogg"},
	}; 

}

// Destructor
j1Scene::~j1Scene()
{
	sceneGuiXMLIndexes.clear(); 
	sceneMusics.clear(); 

}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& node)
{
	LOG("Loading Scene");
	bool ret = true;
	sceneNode = node;
	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{

	return true;

}


// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdate Scene", Profiler::Color::Turquoise);
 

	if (toUpdate == true)
	{
		LoadScene(nextSceneState, true);
		toUpdate = false; 
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Update Scene", Profiler::Color::LawnGreen);
	
	if (App->map->active)
		App->map->Draw();


	int x, y;
	x = y = 0;
	App->input->GetMousePosition(x, y);

	uint wX, wY;

	App->win->GetWindowSize(wX, wY);

	if (App->render->camera.x < 0)
		x += -App->render->camera.x;

		
	/*if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		App->entityFactory->CreateEntity(ENEMY_IMP, x, y, "EnemyIMP");
	}
	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		App->entityFactory->CreateEntity(ENEMY_CACODEMON, x, y, "EnemyCacodemon");
	}
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		App->entityFactory->CreateEntity(ENEMY_BARON_OF_HELL, x, y, "EnemyBaronOfHell");
	}

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		App->entityFactory->CreateEntity(ENEMY_HELL_KNIGHT, x, y, "EnemyHellKnight");
	}
	*/


	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN) // TODO: change ALL debug keys to the proper ones
	{
		App->entityFactory->playerLives = 3; 
		App->gui->UpDateInGameUISlot("LiveCounter", 3);
		App->scene->LoadScene(SceneState::LEVEL1, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN) // TODO: change ALL debug keys to the proper ones
	{
		App->entityFactory->playerLives = 3;
		App->gui->UpDateInGameUISlot("LiveCounter", 3);
		App->scene->LoadScene(state, true);
	}
 
	if (App->input->GetKey(SDL_SCANCODE_KP_3) == KEY_DOWN) // TODO: THIS KEY IS NOT NEEDED
	{
		App->entityFactory->playerLives = 3;
		App->gui->UpDateInGameUISlot("LiveCounter", 3);
		App->scene->LoadScene(SceneState::LEVEL2, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_8) == KEY_DOWN)         // TODO: change ALL debug keys to the proper ones
		SaveLoadLogic(true); 

	if (App->input->GetKey(SDL_SCANCODE_KP_9) == KEY_DOWN)         // TODO: change ALL debug keys to the proper ones
		SaveLoadLogic(false);

	return true;
}

void j1Scene::SaveLoadLogic(bool save)
{
	bool doIt = false; 

	if (GetCurrentSceneTypeGui() == sceneTypeGUI::LEVEL)
		if (App->fade->GetCurrentStep() == fade_step::none)
			if(App->entityFactory->playerAlive == true)
				doIt = true;
		
	if (doIt == true)
	{
		if (save)
		{
			App->SaveGame("save_game.xml");
			previousSceneState = state;
		}
			
		else
		{
			if (App->DoesSaveGameFileExist() == true)
			{
				App->LoadGame("save_game.xml");
				previousSceneState = state;
			}

		}
				
	}

}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate Scene", Profiler::Color::MediumOrchid);
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	App->tex->UnLoad(debug_tex);
	debug_tex = nullptr;


	LOG("Freeing scene");
	return true;
}

bool j1Scene::Save(pugi::xml_node &node) const
{
	savedState = state; 

	// TODO: do I have to preserve the previous state variable?? 
	return true;
}

bool j1Scene::Load(pugi::xml_node &node)
{
	LoadScene(savedState, true, true); 
	return true;
}


void j1Scene::LoadNewMap(const char* mapName)
{
 

	if (App->map->Load(mapName))
	{
		int w, h;
		uchar* data = NULL;
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);
 

		 
	}
}

void j1Scene::UnLoadScene(bool saveLoad)
{

	App->map->Disable();
	App->pathfinding->Disable();
	if (saveLoad == false)
		App->entityFactory->saveGameDeletedEntitiesIDs.clear();   // clear destroyed entities IDs. When saving, do not do it here cause factory needs em, it is cleared there
	App->entityFactory->Disable();
	App->bloodManager->Disable();
	App->particles->Disable();
	App->collision->Disable();
 
	//App->audio->UnLoadAudio(); // why? xd 
 
}

void j1Scene::LoadScene(SceneState sceneState, bool loadGUI, bool saveLoad)
{
	previousSceneState = state; 

	if (App->fade->GetCurrentStep() != fade_step::none)  // prevention
		return; 

	nextSceneState = sceneState; 
	this->loadGUI = loadGUI; 

	UnLoadScene(saveLoad);   // 1) First unload every needed module


	if (saveLoad == false)
	{
		// Make a fade, do NOT create anything until fade allows it 
		App->fade->FadeToBlack(sceneSwapColor);
	}
	else
		CreateScene();
		
		
}


void j1Scene::CreateScene()  // called by fade 
{
	App->audio->PlayMusic(sceneMusics.at(nextSceneState), -1); 

	switch (nextSceneState)
	{                           // 2) Then call load map (which has encapsulated entity data)

	case SceneState::LEVEL1:
		LoadNewMap("maps/level 1.tmx");
		//LoadNewMap("maps/level 1 test.tmx");
		break;

	case SceneState::LEVEL2:
		LoadNewMap("maps/level 2.tmx");
		break;
	default:
		break;
	}

	// 3) then load modules

	//App->audio->Start(); // why? xd 
 
	App->render->ResetCamera();

	if (nextSceneState == SceneState::LEVEL1 || nextSceneState == SceneState::LEVEL2)
	{
		App->pathfinding->Enable();
		App->entityFactory->Enable();
		App->bloodManager->Enable();
		App->particles->Enable();
		App->collision->Enable();

		App->map->active = true;
	}

	// 4) load the GUI when map swap comes from collider win, or ingame esc. Do NOT load it when coming from button (which already executes the load)

	if (loadGUI == true)
		App->gui->LoadGuiDefined(convertSceneTypeToGui(nextSceneState));

	state = nextSceneState;

}

sceneTypeGUI j1Scene::convertSceneTypeToGui(SceneState state)
{

	if ((int)state <= 5)
		return (sceneTypeGUI)(int)state;
	else
		switch (state)
		{
		case SceneState::LEVEL1:
			return sceneTypeGUI::LEVEL;
			break;
		case SceneState::LEVEL2:
			return sceneTypeGUI::LEVEL;
			break;
		}

	return sceneTypeGUI::NO_SCENE;

}

 