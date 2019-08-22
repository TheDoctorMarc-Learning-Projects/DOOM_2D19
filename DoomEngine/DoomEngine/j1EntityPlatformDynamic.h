#ifndef _J1ENTITYPLATFORMDYNAMIC_H__
#define _J1ENTITYPLATFORMDYNAMIC_H__

#include "j1EntityPlatform.h"
#include "j1Scene.h"

enum class AXIS_Movement
{
	VERTICAL,
	HORIZONTAL,
//	DIAGONAL  // we will see 
};

class j1EntityPlatformDynamic : public j1EntityPlatform
{
public:
	j1EntityPlatformDynamic(SDL_Rect placing, int heightLevel, int LevelsUp, int LevelsDown, SceneState level, AXIS_Movement movementType);
	~j1EntityPlatformDynamic();
	
	bool CleanUp() override; 
	bool Update(float dt);
	void SetAreaColPos(); 
	void OnCollision(Collider* c1, Collider* c2) override;
	void OnCollisionExit(Collider* c1, Collider* c2) override;
	void CheckPlatformSameLevel(); 
	void UpdateEntitiesOnTopPositions(bool justOffset = false, float offset = 0.f);


public:
	AXIS_Movement movementType; 
	POINTING_DIR lastPointingDir; 
	uint heighLevelTravel = 0; 


private: 
	bool endReached = false;
	int levelsUp; 
	int levelsDown; 
	Collider* AreaCollider = nullptr;
	int areaExtraSides = 100;  
};

#endif 
