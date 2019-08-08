#ifndef __J1FADETOBLACK_H__
#define __J1FADETOBLACK_H__

#include "j1Module.h"
#include "SDL/include/SDL.h"


struct SDL_Color; 

enum fade_step
{
	none,
	fade_to_black,
	fade_from_black
}; 

class j1FadeToBlack : public j1Module
{
public:
	j1FadeToBlack();
	~j1FadeToBlack();

	bool Start();
	bool Update(float dt);
	bool FadeToBlack(SDL_Color c = {0, 0, 0, 255}, float time = 2.0f);



private:

	fade_step current_step = fade_step::none;

	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
	SDL_Color currentColor; 



public:
	fade_step GetCurrentStep() const { return current_step; }; 

};

#endif 