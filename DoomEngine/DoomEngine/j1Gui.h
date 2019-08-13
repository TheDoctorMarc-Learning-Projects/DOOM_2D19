#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"

#include <list>
#include <map>
#include "UiItem.h"
#include "UiItem_Image.h"
#include "UiItem_Label.h"
#include "UiItem_Checkbox.h"
#include "UiItem_Button.h"
#include "UiItem_Bar.h"
#include "UiItem_Button.h"
#include "UiItem_Face.h"
 

void LoadGui(UiItem* callback = nullptr);
void ExitGame(UiItem* callback = nullptr);
void SetDifficulty(UiItem* callback = nullptr);
 
class j1Gui : public j1Module
{
public:

	j1Gui();
	~j1Gui();

 
	bool Awake(pugi::xml_node&);
	void FillFunctionsMap(); 
	bool Start();
	bool Update(float dt);
//	void DoLogicSelected();

	bool PostUpdate();
	bool CleanUp();

private: 
	void destroyElement(UiItem*);
	 
public: 
	UiItem_Label* AddLabel(std::string name, std::string text, SDL_Color color, TTF_Font* font, p2Point<int> position, UiItem* const parent, float SpriteScale = 0.0f);
	UiItem_Image* AddImage(iPoint position, const SDL_Rect* section, std::string name, UiItem* const parent, bool isTabbable = false, SDL_Texture* specialTex = nullptr, float spriteScale = 0.0f,
		std::string newTextureName = "");
	UiItem_Bar* AddBar(iPoint position, std::string name, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* image_idle, const SDL_Rect* image_hover, UiItem* const parent);  
	UiItem_Button* AddButton(iPoint position, std::string functionName, std::string name, std::string text, SDL_Color color, TTF_Font * font, UiItem* const parent, float spriteScale, SceneState targetScene);
	UiItem* AddEmptyElement(iPoint pos, UiItem* const parent = nullptr);
	UiItem_Checkbox* AddCheckbox(iPoint position, std::string function, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click = nullptr, const SDL_Rect* hover = nullptr, const SDL_Rect* tick_section = nullptr);
	UiItem_Face* AddFace(iPoint position, std::string name, UiItem* const parent, float spriteScale = 0.0f);

	void LoadGuiDefined(sceneTypeGUI targetScene); 
	void ChangeCurrentCanvas(UiItem* newCanvas, bool exists);

	std::map<std::string, void(*)(UiItem* callback)> GetFunctionsMap()
	{
		return functionsMap;
	}


	UiItem* GetCurrentCanvas() const { 
		return currentCanvas; 
	};
 
	std::map<sceneTypeGUI, UiItem*> GetCanvases() const { return canvases; }; 
	SDL_Texture* GetAtlas() const { return atlas; };
	float GetSpriteGlobalScale() const { return spriteScale; };   // TODO: alter this when needed, pass the items a specific scale if needed
	UiItem* GetItemByName(std::string name, UiItem* parent = nullptr) const;

	void UpDateInGameUISlot(std::string name, float newValue = 0.0f, SDL_Rect newSection = { 0, 0, 0, 0 });
	void UpdateDeathTimer(int value = -1);     // it is here just not to call the above function in app, maybe it is cleaner to call it from here; 

private: 
	void initializeGUI(); 
	void LoadXMLGUI(pugi::xml_node& menuNode);
	void ResetInGameUI(); 

	
private:
	SDL_Texture* atlas = nullptr;
	UiItem* currentCanvas = nullptr;
	UiItem* selected_object = nullptr;
	std::list<UiItem*>	listOfItems;
	std::map<sceneTypeGUI, UiItem*> canvases;
	std::map<std::string, void(*)(UiItem* callback)> functionsMap;  // the functions have callback. So I can call them in button cpp etc without if / elses 

	std::map<std::string, int> defaultInGameStats; 

	std::string atlas_file_name;
	float spriteScale = 0.0f; 



public: 
	bool debug = false;

 
 
};






#endif


