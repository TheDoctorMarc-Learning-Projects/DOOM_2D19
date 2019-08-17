#ifndef UIITEM_LABEL_H
#define UIITEM_LABEL_H

#include "UiItem.h"
#include <string>
#include "p2Point.h"
#include "SDL_ttf/include/SDL_ttf.h"
#include "j1PerfTimer.h"

class UiItem_Label : public UiItem
{
public:
	UiItem_Label(std::string name, std::string text, SDL_Color color, TTF_Font * font, p2Point<int> position, UiItem*const parent, float SpriteScale, bool resetable);
	~UiItem_Label();

	/*UiItem_Label(std::string text, SDL_Color color, TTF_Font * font, bool typewriter, p2Point<int> position, UiItem*const parent);*/
	/*bool ChangeTextureHover(const std::string * textHover, const SDL_Color* color, const TTF_Font* font);
	bool ChangeTextureIdle(const std::string * textHover, const SDL_Color* color, const TTF_Font* font);*/
	bool ChangeTextureIdle(std::string textIdle, const SDL_Color* color, const TTF_Font* font);
	void Draw();
	void CleanUp();
	//bool Update(float dt) override;
	void DoLogicHovered();
	void DoLogicAbandoned();


public:
	std::string text;
protected:

	SDL_Color color;
	TTF_Font* font = nullptr; 
	SDL_Texture* texture = nullptr;
 
};
#endif
