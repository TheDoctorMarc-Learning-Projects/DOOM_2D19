#ifndef __J1BLOODMANAGER_H__
#define __J1BLOODMANAGER_H__

#include "j1Module.h"
//#include "j1Entity.h"

#include <vector>
#include <list>
#include "Color.h"
#include "j1Collision.h"
#include "j1EntityBloodDrop.h"
#include <random>


#define bloodCubeSideSize 2

#define bloodModuleSpeed 10.5f

class j1EntityBloodDrop; 

struct bloodStream
{
	std::list<j1EntityBloodDrop*> myBloodDrops;
};

enum BloodStreamType
{
	TARGETED,
	RANDOM 
};

class j1BloodManager : public j1Module
{
public:
	j1BloodManager();
	~j1BloodManager();

	bool Awake(pugi::xml_node& node);
	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	void Draw(); 
	bool CleanUp();     // blood are just quads drawn in render, no texture indeed

	void CreateTargetedBloodSteam(SDL_Rect enemyRect, float damage); // TODO: direction(base, and then a cone speed between them, with a 2D sratings area)
	void CreateRandomBloodStream(SDL_Rect enemyRect, float centerAreaScaleFactor, uint numberOfBloodDrops = 10); 
	SDL_Rect GetRectCentralArea(SDL_Rect enemyRect, float centerAreaScaleFactor);
	Color GenerateColorForBloodDrop();   // TODO: std random with a defined spectrum 
	fPoint GenerateRandomSpeedForBloodDrop(); 
	fPoint GenerateTargetedSpeedForBloodDrop();

	float GetRandomValue(float v1, float v2);
	int GetRandomIntValue(int v1, int v2);
private: 

	std::list<bloodStream*> bloodStreams;   // draw them if on camera only
	std::random_device rd;
	std::mt19937 engine;



};




#endif  