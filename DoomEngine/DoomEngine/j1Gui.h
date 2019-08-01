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


void LoadGui(); 
 
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
	UiItem_Label* AddLabel(std::string text, SDL_Color color, TTF_Font* font, p2Point<int> position, UiItem* const parent);
	UiItem_Image* AddImage(iPoint position, const SDL_Rect* section, std::string& name, UiItem* const parent, bool isTabbable = false);
	UiItem_Bar* AddBar(iPoint position, std::string name, const SDL_Rect* section, const SDL_Rect* thumb_section, const SDL_Rect* image_idle, const SDL_Rect* image_hover, UiItem* const parent); // , TypeBar type = VERTICAL);
	UiItem_Button* AddButton(iPoint position, std::string function, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click = nullptr, const SDL_Rect* hover = nullptr);
	UiItem* AddEmptyElement(iPoint pos, UiItem* const parent = nullptr);
	UiItem_Checkbox* AddCheckbox(iPoint position, std::string function, std::string name, const SDL_Rect* idle, UiItem* const parent, const SDL_Rect* click = nullptr, const SDL_Rect* hover = nullptr, const SDL_Rect* tick_section = nullptr);

 
	std::map<std::string, void(*)()> GetFunctionsMap()
	{
		return functionsMap; 
	}
 
private:
	SDL_Texture* atlas = nullptr;
	UiItem* currentCanvas = nullptr;
	UiItem* selected_object = nullptr;
	std::list<UiItem*>	listOfItems;
	std::map<std::string, void(*)()> functionsMap;

	std::string atlas_file_name;

public: 
	bool debug = false;

 
 
};






#endif


