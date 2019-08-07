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
#include "j1Gui.h"
                                    
#include "Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
	state = SceneState::LEVEL1;



	sceneGuiXMLIndexes =
	{
		 {sceneTypeGUI::LEVEL, "InGameUI"},
	};


	// TODO: keep updating this scene-gui index map
}

// Destructor
j1Scene::~j1Scene()
{
	sceneGuiXMLIndexes.clear(); 
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
	LoadScene(SceneState::LEVEL2, true);  // todo, do not load anything at first, rather main menu :)

	return true;

}


// Called each loop iteration
bool j1Scene::PreUpdate()
{
	BROFILER_CATEGORY("PreUpdate Scene", Profiler::Color::Turquoise);
	// debug pathfing ------------------


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

		
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
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


	if (App->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
	{
		UnLoadScene(); 
	}

	if (App->input->GetKey(SDL_SCANCODE_KP_1) == KEY_DOWN)
	{
		LoadScene(SceneState::LEVEL1, true);
	}


	if (App->input->GetKey(SDL_SCANCODE_KP_2) == KEY_DOWN)
	{
		LoadScene(SceneState::LEVEL2, true);
	}


	return true;
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

	return true;
}

bool j1Scene::Load(pugi::xml_node &node)
{

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

void j1Scene::UnLoadScene()
{
	if (App->map->IsEnabled())
		App->map->Disable();

	if (App->pathfinding->IsEnabled())
		App->pathfinding->Disable();

	if (App->entityFactory->IsEnabled())
		App->entityFactory->Disable();

	if (App->bloodManager->IsEnabled())
		App->bloodManager->Disable();

	if (App->particles->IsEnabled())
		App->particles->Disable();

	if (App->collision->IsEnabled())
		App->collision->Disable();

 
	App->audio->UnLoadAudio(); 
 
}

void j1Scene::LoadScene(SceneState sceneState, bool loadGUI)
{
	nextSceneState = sceneState; 

	UnLoadScene();   // 1) First unload every needed module

	switch (sceneState)
	{                                                          // 2) Then call load map (which has encapsulated entity data)

	case SceneState::LEVEL1: 
		App->audio->PlayMusic("sound/music/Arch Enemy First Day In Hell.ogg", -1);  
		LoadNewMap("maps/level 1.tmx");                           
		break;

	case SceneState::LEVEL2:
		LoadNewMap("maps/level 2.tmx"); // LoadNewMap("maps/level 1 for testing.tmx");
		App->audio->PlayMusic("sound/music/soil-halo.ogg", -1);
		break;
	default:
		break;
	}

	// 3) then load modulesa
 
	App->audio->Start();
	App->render->ResetCamera(); 

	if (sceneState == SceneState::LEVEL1 || sceneState == SceneState::LEVEL2)
	{
		if (App->pathfinding->IsEnabled() == false)
			App->pathfinding->Enable();

		if (App->entityFactory->IsEnabled() == false)
			App->entityFactory->Enable();

		if (App->bloodManager->IsEnabled() == false)
			App->bloodManager->Enable();

		if (App->particles->IsEnabled() == false)
			App->particles->Enable();

		if (App->collision->IsEnabled() == false)
			App->collision->Enable();


		App->map->active = true;
	}
 
	// load the GUI when map swap comes from collider win, or ingame esc. Do NOT load it when coming from button (which already executes the load)

	if(loadGUI == true)                                   
		App->gui->LoadGuiDefined(convertSceneTypeToGui(sceneState));
	
	state = sceneState; 

}


sceneTypeGUI j1Scene::convertSceneTypeToGui(SceneState state)
{

	switch (state)
	{
	case SceneState::MAINMENU:
		return sceneTypeGUI::MAINMENU; 
		break;
	case SceneState::LEVEL1:
		return sceneTypeGUI::LEVEL;
		break;
	case SceneState::LEVEL2:
		return sceneTypeGUI::LEVEL;
		break;
	case SceneState::CREDITS:
		return sceneTypeGUI::CREDITS;
		break;
	case SceneState::SETTINGS:
		return sceneTypeGUI::SETTINGS;
		break;
	case SceneState::NO_SCENE:
		return sceneTypeGUI::NO_SCENE;
		break;
	default:
		return sceneTypeGUI::NO_SCENE;
		break;
	}
		

}

 