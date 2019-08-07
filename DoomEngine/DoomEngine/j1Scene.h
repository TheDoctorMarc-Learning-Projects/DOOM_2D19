#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>
#include <map>

#define MAX_ALPHA 255

struct SDL_Texture;

enum class SceneState  // the proper scene state
{
	MAINMENU,
	LEVEL1,
	LEVEL2,
	CREDITS,
	SETTINGS,
	NO_SCENE	
};


enum class sceneType   // two base types so that the scene transitions code is more generic
{
	MENU,
	LEVEL
};

enum class sceneTypeGUI    // coordination with GUI needed for a generic XML load 
{
	MAINMENU,
	LEVEL,
	CREDITS,
	SETTINGS,
	NO_SCENE
};




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
private:
	SDL_Texture* debug_tex = nullptr;

	SceneState state = SceneState::LEVEL1;
	SceneState nextSceneState = SceneState::LEVEL1; 


	sceneTypeGUI convertSceneTypeToGui(SceneState state); 

public:
	pugi::xml_node sceneNode;

	std::map<sceneTypeGUI, std::string> sceneGuiXMLIndexes;
 

	void LoadNewMap(const char* mapName);
	void UnLoadScene();
	void LoadScene(SceneState sceneState, bool loadGUI);  

};

#endif // __j1SCENE_H__