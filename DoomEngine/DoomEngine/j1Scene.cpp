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
		 {sceneTypeGUI::INGAME, "InGameUI"},
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
	// TODO: call load new map in each case --> do not do it in load / unload, or do not do it here if it is already being done there the previous frame


	// TODO: WHY THE HELL is this loaded here? And not in audio start haha. It is borderline idiotic :/ (punishes himself) :/ 

	 
	LoadScene(SceneState::LEVEL2, sceneType::LEVEL);
 


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
	//	LoadScene(SceneState::LEVEL2, sceneType::LEVEL); 
		UnLoadScene(); 
	}

	if (App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
	{
		LoadScene(SceneState::LEVEL1, sceneType::LEVEL);
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

void j1Scene::LoadScene(SceneState sceneState, sceneType menuLevel)
{
	UnLoadScene();   // 1) First unload every needed module

	switch (sceneState)
	{
	case SceneState::STARTMENU:
 
		break;

	case SceneState::LEVEL1: 
		App->audio->PlayMusic("sound/music/soil-halo.ogg", -1);  
		LoadNewMap("maps/level 1.tmx");                           // 2) Then call load map (which has encapsulated entity data)
		break;

	case SceneState::LEVEL2:
		LoadNewMap("maps/level 1 for testing.tmx");
		App->audio->PlayMusic("sound/music/stricken.ogg", -1);
		break;
	default:
		break;
	}

	// 3) then load modules, the previous encapsulated entity data... 

 
	App->audio->Start();

	if (menuLevel == sceneType::LEVEL)                  
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
 

	state = sceneState; 
 

}



