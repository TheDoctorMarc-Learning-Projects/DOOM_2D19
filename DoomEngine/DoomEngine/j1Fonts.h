#ifndef __j1FONTS_H__
#define __j1FONTS_H__

#include "j1Module.h"
#include "SDL\include\SDL_pixels.h"


#include <vector>
#include <list>

#define DEFAULT_FONT "fonts/open_sans/OpenSans-Regular.ttf"
#define DEFAULT_FONT_SIZE 12

struct SDL_Texture;
struct _TTF_Font;

class j1Fonts : public j1Module
{
public:

	j1Fonts();

	// Destructor
	virtual ~j1Fonts();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Load Font
	_TTF_Font* const Load(const char* path, int size = 12);

	// Create a surface from text
	SDL_Texture* Print(const char* text, SDL_Color color = { 255, 255, 255, 255 }, _TTF_Font* font = NULL);

	bool CalcSize(const char* text, int& width, int& height, _TTF_Font* font = NULL) const;

public:

	std::vector<_TTF_Font*>	fonts;



	_TTF_Font*				defaultFont = nullptr;
	_TTF_Font*			    openSansBold12 = nullptr;
	_TTF_Font*			    openSansBold18 = nullptr;
	_TTF_Font*			    openSansBold36 = nullptr;
	_TTF_Font*			    openSansSemiBold24 = nullptr;
	_TTF_Font*				openSansSemiBold36 = nullptr;
	_TTF_Font*				piecesofEight36 = nullptr;
	_TTF_Font*				piecesofEight48 = nullptr;
	_TTF_Font*				piecesofEight24 = nullptr;
	_TTF_Font*				piecesofEight18 = nullptr;

	// hitpoint labels
	_TTF_Font*              shatterBoxx36 = nullptr;
	_TTF_Font*              shatterBoxx48 = nullptr;

	_TTF_Font* knightsQuest18 = nullptr;
	_TTF_Font* knightsQuest24 = nullptr;


	_TTF_Font* weekSRB18 = nullptr;
	_TTF_Font* weekSRB24 = nullptr;
};


#endif // __j1FONTS_H__