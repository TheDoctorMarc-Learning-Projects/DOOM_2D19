#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>
#include <map>

#define MAX_ALPHA 255

struct SDL_Texture;

enum class SceneState : int // the proper scene state
{
	NO_SCENE,
	MAINMENU,
	CREDITS,
	LEVEL1,
	LEVEL2,
	SETTINGS,
	IN_GAME_SETTINGS
		
};


enum class sceneTypeGUI : int    // coordination with GUI needed for a generic XML load 
{
	NO_SCENE,
	MAINMENU,
	CREDITS,
	LEVEL,
	SETTINGS
};


struct SDL_Color; 

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& node);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	SceneState GetCurrentSceneState() const { return state; }; 
	SceneState GetNextSceneState() const { return nextSceneState; };  // when transitioning 
	sceneTypeGUI convertSceneTypeToGui(SceneState state);

private:
	SDL_Texture* debug_tex = nullptr;
	SceneState state = SceneState::MAINMENU;
	SceneState nextSceneState = SceneState::NO_SCENE; 

	SDL_Color sceneSwapColor = { 30, 0, 0, 255 }; 
	bool loadGUI = false; 
	

public:
	pugi::xml_node sceneNode;

	std::map<sceneTypeGUI, std::string> sceneGuiXMLIndexes;
	std::map<SceneState, const char*> sceneMusics;   // will come in handy, as usual with maps :)
 

	void LoadNewMap(const char* mapName);
	void UnLoadScene();
	void LoadScene(SceneState sceneState, bool loadGUI);  
	void CreateScene();

};

#endif // __j1SCENE_H__