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
	DIFFICULTY_SELECTOR,
	CREDITS,
	SETTINGS,
	IN_GAME_SETTINGS,
	LEVEL1,
	LEVEL2
};


enum class sceneTypeGUI : int    // coordination with GUI needed 
{
	NO_SCENE,
	MAINMENU,
	DIFFICULTY_SELECTOR,
	CREDITS,
	SETTINGS,
	IN_GAME_SETTINGS,  // it is different from settings because it has back to play and back to main :/
	LEVEL
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

	void SaveLoadLogic(bool save); 

	bool Save(pugi::xml_node&) const;
	bool Load(pugi::xml_node&);

	void SetSceneState(SceneState state) { this->state = state; }; 
	void SetPreviousSceneState(SceneState state) { this->previousSceneState = state; }; 

	SceneState GetPreviousSceneState() const { return previousSceneState; };
	SceneState GetCurrentSceneState() const { return state; }; 
	SceneState GetNextSceneState() const { return nextSceneState; }; 

	sceneTypeGUI convertSceneTypeToGui(SceneState state);
	sceneTypeGUI GetCurrentSceneTypeGui() { return convertSceneTypeToGui(state); };


private:
	SDL_Texture* debug_tex = nullptr;
	SceneState state = SceneState::MAINMENU;
	SceneState nextSceneState = SceneState::NO_SCENE; 
	SceneState previousSceneState = SceneState::NO_SCENE;

	mutable SceneState savedState = SceneState::NO_SCENE;

	SDL_Color sceneSwapColor = { 30, 0, 0, 255 }; 
	bool loadGUI = false; 
	bool toUpdate = false; 

public:
	pugi::xml_node sceneNode;

	std::map<sceneTypeGUI, std::string> sceneGuiXMLIndexes;
	std::map<SceneState, const char*> sceneMusics;   // will come in handy, as usual with maps :)
 

	void LoadNewMap(const char* mapName);
	void UnLoadScene(bool saveLoad = false);
	void LoadScene(SceneState sceneState, bool loadGUI, bool saveLoad = false);  
	void CreateScene();
	void SetTargetScene(SceneState sceneState)
	{
		nextSceneState = sceneState; 
		toUpdate = true; 
	}
};

#endif // __j1SCENE_H__