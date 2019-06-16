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

#include "Brofiler/Brofiler.h"
#include "SDL_mixer/include/SDL_mixer.h"
#include "j1ParticlesClassic.h"
j1Scene::j1Scene() : j1Module()
{
	name.assign("scene");
	state = SceneState::STARTMENU;
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

	/*if (debug_tex == nullptr)
		debug_tex = App->tex->Load("maps/path2.png");*/   // change this, or add this, in Game Folder

	if (state == SceneState::LEVEL1)
	{
		//App->audio->PlayMusic("audio/music/FFDI_Theme_14.ogg", -1);   // change with new music


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
	// how to get joysticks for correct UI navigation/whatever needed situation examples --------
	if (App->input->GetJoystickPulsation(JOY_STICK_LEFT, JOYSTICK_DIR_UP) == KEY_DOWN)
	{
		//LOG("LEFT JOYSTICK, UP PRESS");
	}
	if (App->input->GetJoystickPulsation(JOY_STICK_LEFT, JOYSTICK_DIR_RIGHT) == KEY_DOWN)
	{
		//LOG("LEFT JOYSTICK, RIGHT PRESS");
	}
	if (App->input->GetJoystickPulsation(JOY_STICK_LEFT, JOYSTICK_DIR_RIGHT) == KEY_UP)
	{
		//LOG("LEFT JOYSTICK, RIGHT UNPRESS");
	}
	if (App->input->GetJoystickPulsation(JOY_STICK_RIGHT, JOYSTICK_DIR_LEFT) == KEY_DOWN)
	{
		//LOG("RIGHT JOYSTICK, LEFT PRESS");
	}
	if (App->input->GetJoystickPulsation(JOY_STICK_RIGHT, JOYSTICK_DIR_UP) == KEY_DOWN)
	{
		//LOG("RIGHT JOYSTICK, UP PRESS");
	}
	if (App->input->GetJoystickPulsation(JOY_STICK_RIGHT, JOYSTICK_DIR_DOWN) == KEY_REPEAT)
	{
		//LOG("RIGHT JOYSTICK, DOWN REPEAT");
	}
	// etc
	// -------------------------------------------------------------------------------------------

	int mx, my;

	App->input->GetMousePosition(mx, my);
	//iPoint mousePos = App->render->ScreenToWorld(mx, my);
	//LOG("mousePos: %i,%i", mousePos.x, mousePos.y);
//	mousePos = App->map->WorldToMap(mousePos.x, mousePos.y);
	//LOG("mousePosMap: %i,%i", mousePos.x, mousePos.y);
	
	
	if (App->map->active)
		App->map->Draw();

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	BROFILER_CATEGORY("PostUpdate Scene", Profiler::Color::MediumOrchid);
	bool ret = true;


	// TODO: redo this to ESC the game: 

	/*if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	if (exitGame)  
		return false;*/

	

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
		                                                // TODO: if I add new modules, enable them if needed -> DON'T DO IT HERE, its disgusting, do it in start :)) 
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



