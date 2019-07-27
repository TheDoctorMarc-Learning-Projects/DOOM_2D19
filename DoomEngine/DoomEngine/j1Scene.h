#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"
#include <list>

#define MAX_ALPHA 255

struct SDL_Texture;

enum class SceneState
{
	STARTMENU,
	LEVEL1,
	LEVEL2,
};

enum class sceneType
{
	MENU,
	LEVEL
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



public:

	SceneState state = SceneState::LEVEL1;
	
private:
	SDL_Texture* debug_tex = nullptr;
	
	pugi::xml_node sceneNode;
	
public:

	void LoadNewMap(const char* mapName);
	void UnLoadScene();
	void LoadScene(SceneState sceneState, sceneType menuLevel);

};

#endif // __j1SCENE_H__