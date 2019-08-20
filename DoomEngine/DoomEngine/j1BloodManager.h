#ifndef __J1BLOODMANAGER_H__
#define __J1BLOODMANAGER_H__

#include "j1Module.h"
//#include "j1Entity.h"

#include <vector>
#include <list>
#include "Color.h"
#include "j1Collision.h"
#include "j1EntityBloodDrop.h"
#include "j1EntityFactory.h"
#include <random>


#define bloodCubeSideSize 3
#define bloodModuleSpeed 12.5f
#define randomBloodSpeedXFactor 0.6f
#define bloodAmountPerDamageProportion 0.34f

class j1EntityBloodDrop; 

class bloodStream
{
public: 
	std::list<j1EntityBloodDrop*> myBloodDrops;
public: 
	
	bool Save(pugi::xml_node& node) const {
		for(const auto& drop : myBloodDrops)
			drop->Save((pugi::xml_node&)node.append_child("blood_drop"));
		return true; 
	}
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
	bool Draw(); 
	bool CleanUp();     // blood are just quads drawn in render, no texture indeed


	bool Load(pugi::xml_node& node); 
	bool Save(pugi::xml_node& node) const; 

public: 

	void CreateTargetedBloodSteam(SDL_Rect enemyRect, float damage, uint numberOfBloodDrops, fPoint shotSpeed); // TODO: direction(base, and then a cone speed between them, with a 2D sratings area)
	void CreateRandomBloodStream(SDL_Rect enemyRect, float centerAreaScaleFactor, uint numberOfBloodDrops = 10); 
	SDL_Rect GetRectCentralArea(SDL_Rect enemyRect, float centerAreaScaleFactor);
	Color GenerateColorForBloodDrop(); 
	fPoint GenerateRandomSpeedForBloodDrop(); 
	fPoint GenerateTargetedSpeedForBloodDrop(fPoint shotSpeed);

	uint CalculateNumberOfBloodDropsToBeSpawned(float damage, float shotsPerSec);

	float GetRandomValue(float v1, float v2);
	int GetRandomIntValue(int v1, int v2);
private: 

	std::list<bloodStream*> bloodStreams;   // draw them if on camera only
	std::random_device rd;
	std::mt19937 engine;



};




#endif  