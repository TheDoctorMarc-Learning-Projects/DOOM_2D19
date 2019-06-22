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
#include "j1ParticlesClassic.h"
#include "j1Collision.h"        
#include "j1EntityFactory.h"
                                                     // TODO: later on, check if any library is not neede here 
#include "Brofiler/Brofiler.h"

j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
	state = SceneState::LEVEL1;
}

// Destructor
j1Scene::~j1Scene()
{}

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


	App->audio->LoadFx("sound/fx/bigenemygrunt.wav", "bigenemygrunt"); 
	


	


	if (state == SceneState::LEVEL1)
	{
		//App->audio->PlayMusic("audio/music/FFDI_Theme_14.ogg", -1);   // change with new music
		LoadNewMap("maps/level 1.tmx");
	}

	if (state == SceneState::LEVEL2)
	{

		//App->audio->PlayMusic("audio/music/BRPG_Hell_Spawn_FULL_Loop.ogg", -1);  // change with new music

	}

	if (state == SceneState::STARTMENU)
	{

		//App->audio->PlayMusic("audio/music/menu_1.0.ogg");   // change with new music
	}


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


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		App->audio->PlayFx("bigenemygrunt"); 

	

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
;
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

		// not yet, check this later: 

		/*// re set entities data map (create or delete/create if we have a previous one)    
		App->entityFactory->CreateEntitiesDataMap(App->map->data.width * 2, App->map->data.height * 2);*/
	}
}

void j1Scene::UnLoadScene()
{
	// TODO: Clean floor collider, clean all objects colliders 
	
	// TODO: if I add new modules, disable them here if needed 
	if (App->map->IsEnabled())
		App->map->Disable();

	if (App->pathfinding->IsEnabled())
		App->pathfinding->Disable();
	

	App->audio->UnLoadAudio();
}

void j1Scene::LoadScene(SceneState sceneState)
{


	UnLoadScene();


	switch (sceneState)
	{
	case SceneState::STARTMENU:
		state = SceneState::STARTMENU;
		break;

	case SceneState::LEVEL1:
		                                                // TODO: if I add new modules, enable them if needed  
		state = SceneState::LEVEL1;
	                                               
		App->pathfinding->Enable();
		App->map->active = true;
		//LoadNewMap("maps/Level1_Final_Borders_Faked.tmx");  // TODO: change map and sound
		


		break;

	case SceneState::LEVEL2:
		state = SceneState::LEVEL2;
		
		App->pathfinding->Enable();
	
		App->map->active = true;
		/*App->audio->PlayMusic("audio/music/BRPG_Hell_Spawn_FULL_Loop.ogg");
		LoadNewMap("maps/Level2_rework.tmx");*/                                          // TODO: change map and sound
		
		break;
	default:
		break;
	}

	Start();

}



