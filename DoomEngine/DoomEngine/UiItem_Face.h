#ifndef UIITEM_FACE_H
#define UIITEM_FACE_H

#include "UiItem.h"
#include "p2Point.h"
#include "p2Animation.h"
#include "j1Textures.h"
#include "p2Log.h"

struct SDL_Texture;
 

struct animTimer
{
	j1Timer timer;
	uint actionTime = (uint)INT_MAX; // how much time will the face remain there 
};


struct advancedAnim
{
	Animation anim;
	animTimer myTimer;
};


struct Anims
{
	advancedAnim idle;
	advancedAnim weapon;
	advancedAnim kill;
	advancedAnim damaged;

	std::map<std::string, advancedAnim> karmaMap; 
};

// for save and load 

struct LastAnimData
{
	uint lastHealthLevel = 666U; 
	std::string lastKarma = ""; 
};

class UiItem_Face : public UiItem
{

public:
	UiItem_Face(iPoint position, std::string& name, UiItem* const parent, float spriteScale = 0.0f);
	~UiItem_Face();
	void Draw();
	void CleanUp();
	void FIllMaps(); 
	void SetCurrentAnim(std::string);  // pass it the player life, it will convert it to a life level. Also, a string with "idle", "kill" etc


	bool Load(pugi::xml_node& node);
	bool Save(pugi::xml_node& node) const;


private:
	LastAnimData lastAnim; 
	advancedAnim currentAnimation;
	Animation death; 
	Animation win;         // death and win work differently 
	bool toUpdate = false; 

	std::map<uint, Anims> animMap; // the uint is the player life level: 1, 2, 3... : depending on the player life a different set of anims is assigned
	 
};


#endif

