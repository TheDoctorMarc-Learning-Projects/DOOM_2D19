#include <math.h>

#include "j1App.h"
#include "j1FadeToBlack.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"

j1FadeToBlack::j1FadeToBlack()
{
	name.assign("fade");
}

j1FadeToBlack::~j1FadeToBlack()
{}

// Load assets
bool j1FadeToBlack::Start()
{
	uint width, height;
	App->win->GetWindowSize(width, height);
	int scale = App->win->GetScale();

	screen = { 0, 0, (int)width * scale, (int)height * scale };


	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

// Update: draw background
bool j1FadeToBlack::Update(float dt)
{
	if (current_step == fade_step::none)
		return true;

	Uint32 now = SDL_GetTicks() - start_time;
	float normalized = MIN(1.0f, (float)now / (float)total_time);

	switch (current_step)
	{
	case fade_step::fade_to_black:
	{
		if (now >= total_time)
		{
			// create the scene
			App->scene->CreateScene();

			// ---
			total_time += total_time;
			start_time = SDL_GetTicks();
			current_step = fade_step::fade_from_black;

			
		}
	} break;

	case fade_step::fade_from_black:
	{
		normalized = 1.0f - normalized;

		if (now >= total_time)
			current_step = fade_step::none;
	} break;
	}

	// Finally render the black square with alpha on the screen
	SDL_SetRenderDrawColor(App->render->renderer, currentColor.r, currentColor.g, currentColor.b, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

// Fade to black
bool j1FadeToBlack::FadeToBlack(SDL_Color color, float time)
{
	bool ret = false;
	
	if (current_step == fade_step::none)
	{
		currentColor = color; 
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;
	}

	return ret;
}