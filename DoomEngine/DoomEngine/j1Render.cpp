#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Entity.h"
#include "j1EntityPlayer.h"
#include "j1EntityPlatformDynamic.h"

j1Render::j1Render() : j1Module()
{
	name.assign("renderer");
	background.r = 0;
	background.g = 0;
	background.b = 0;
	background.a = 0;
}

// Destructor
j1Render::~j1Render()
{}

// Called before render is available
bool j1Render::Awake(pugi::xml_node& config)
{
	LOG("Create SDL rendering context");
	bool ret = true;
	// load flags
	Uint32 flags = SDL_RENDERER_ACCELERATED;

	if(config.child("vsync").attribute("value").as_bool() == true)
	{
		flags |= SDL_RENDERER_PRESENTVSYNC;
		LOG("Using vsync");
		App->vsync = true; 
	}

	renderer = SDL_CreateRenderer(App->win->window, -1, flags);
	uint w, h;
	App->win->GetWindowSize(w, h);
	SDL_RenderSetLogicalSize(renderer, w, h);

	if(renderer == NULL)
	{
		LOG("Could not create the renderer! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		camera.w = App->win->screen_surface->w;
		camera.h = App->win->screen_surface->h;
		camera.x = 0;
		camera.y = 0;
	}



	return ret;
}

// Called before the first frame
bool j1Render::Start()
{
	LOG("render start");
	// back background
	SDL_RenderGetViewport(renderer, &viewport);
	return true;
}

// Called each loop iteration
bool j1Render::PreUpdate()
{
	SDL_RenderClear(renderer);
	return true;
}

bool j1Render::Update(float dt)
{
	if (scrollState == cameraScrollState::SCROLLING)
		DoScroll(); 
	
	return true;
}

bool j1Render::PostUpdate()
{
	
	SDL_SetRenderDrawColor(renderer, background.r, background.g, background.g, background.a);
	SDL_RenderPresent(renderer);
	return true;
}

// Called before quitting
bool j1Render::CleanUp()
{
	LOG("Destroying SDL render");
	SDL_DestroyRenderer(renderer);
	return true;
}

void j1Render::SetBackgroundColor(SDL_Color color)
{
	background = color;
}

void j1Render::SetTextureColor(SDL_Texture* tex, uint r, uint g, uint b)
{
	SDL_SetTextureColorMod(tex, r, g, b);
}

void j1Render::SetTextureAlpha(SDL_Texture* tex, uint alpha)
{
	SDL_SetTextureAlphaMod(tex, alpha);
}

void j1Render::SetViewPort(const SDL_Rect& rect)
{
	SDL_RenderSetViewport(renderer, &rect);
}

void j1Render::ResetViewPort()
{
	SDL_RenderSetViewport(renderer, &viewport);
}

iPoint j1Render::ScreenToWorld(int x, int y) const
{
	iPoint ret;
	int scale = App->win->GetScale();
	
	ret.x = (x - camera.x / scale);
	ret.y = (y - camera.y / scale);

	return ret;
}

iPoint j1Render::WorldToScreen(int x, int y, bool descriptions) const
{

	
		return iPoint(x * App->win->GetScale() + camera.x, y * App->win->GetScale() + camera.y);
	
		//return iPoint(x * App->win->GetScale(), y * App->win->GetScale());
}

bool j1Render::IsOnCamera(const int & x, const int & y, const int & w, const int & h) const
{
	int scale = App->win->GetScale();

	SDL_Rect r = { x*scale,y*scale,w*scale,h*scale };
	SDL_Rect cam = { -camera.x,-camera.y,camera.w,camera.h};

	return SDL_HasIntersection(&r, &cam);
}


bool j1Render::isRectOnCamera(const SDL_Rect& dimensions) const
{
	int scale = App->win->GetScale();

	SDL_Rect r = { dimensions.x*scale,dimensions.y*scale,dimensions.w*scale,dimensions.h*scale };
	SDL_Rect cam = { -camera.x,-camera.y,camera.w,camera.h };

	return SDL_HasIntersection(&r, &cam);
}




bool j1Render::Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section, float speed, SDL_RendererFlip flip, float spriteScale, double angle, int pivot_x, int pivot_y, bool useWindowScale) const
{
	bool ret = true;
	uint scale = App->win->GetScale();
	SDL_Rect rect;
	if (useWindowScale)
	{
		rect.x = (int)(camera.x * speed) + x * scale;
		rect.y = (int)(camera.y * speed) + y * scale;
	}
	else
	{
		rect.x = (int)(camera.x * speed) + x;
		rect.y = (int)(camera.y * speed) + y;
	}


	if (section != NULL)
	{
		rect.w = section->w * spriteScale;
		rect.h = section->h * spriteScale;
	}
	else
	{
		SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h);
	}

	rect.w *= scale;
	rect.h *= scale;

	SDL_Point* p = NULL;
	SDL_Point pivot;

	if (pivot_x != INT_MAX && pivot_y != INT_MAX)
	{
		pivot.x = pivot_x;
		pivot.y = pivot_y;
		p = &pivot;
	}

	if (SDL_RenderCopyEx(renderer, texture, section, &rect, angle, p, flip) != 0)
	{
		LOG("Cannot blit to screen. SDL_RenderCopy error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}
 

bool j1Render::DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool filled, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	SDL_Rect rec(rect);
	if(use_camera)
	{
		rec.x = (int)(camera.x + rect.x * scale);
		rec.y = (int)(camera.y + rect.y * scale);
		rec.w *= scale;
		rec.h *= scale;
	}

	int result = (filled) ? SDL_RenderFillRect(renderer, &rec) : SDL_RenderDrawRect(renderer, &rec);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;

	if(use_camera)
		result = SDL_RenderDrawLine(renderer, camera.x + x1 * scale, camera.y + y1 * scale, camera.x + x2 * scale, camera.y + y2 * scale);
	else
		result = SDL_RenderDrawLine(renderer, x1 * scale, y1 * scale, x2 * scale, y2 * scale);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}

bool j1Render::DrawCircle(int x, int y, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a, bool use_camera) const
{
	bool ret = true;
	uint scale = App->win->GetScale();

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, r, g, b, a);

	int result = -1;
	SDL_Point points[360];

	float factor = (float)M_PI / 180.0f;

	for(uint i = 0; i < 360; ++i)
	{
		if (use_camera)
		{
			points[i].x = (int)(x + radius * cos(i * factor));
			points[i].y = (int)(y + radius * sin(i * factor));
		}
		else
		{
			points[i].x = (int)((x + camera.x) + radius * cos(i * factor));
			points[i].y = (int)((y + camera.y) + radius * sin(i * factor));
		}
	}

	result = SDL_RenderDrawPoints(renderer, points, 360);

	if(result != 0)
	{
		LOG("Cannot draw quad to screen. SDL_RenderFillRect error: %s", SDL_GetError());
		ret = false;
	}

	return ret;
}


void j1Render::SetCameraScroll(cameraScrollType scrollType, direction dir, int speed, int worldDistance)
{
	if (scrollState != cameraScrollState::SCROLLING)
	{

		scrollValues.speed = speed; 
		scrollValues.worldDistance = worldDistance; 
		scrollValues.originPos = camera.x; 


		scrollState = cameraScrollState::SCROLLING; 

	}

}



void j1Render::DoScroll()
{
	int captureX = camera.x; 

	captureX += scrollValues.speed; 

	if (captureX > 0 || captureX < -App->map->mapLimitXWorldPos + camera.w)     
	{
		scrollState = cameraScrollState::AVAILABLE;
		scrollValues.speed = 0; 
		return; 
	}


	camera.x = captureX;

	if (abs(camera.x - scrollValues.originPos) >= scrollValues.worldDistance)
	{
		scrollState = cameraScrollState::AVAILABLE;
		scrollValues.speed = 0;
	}
		

}



bool j1Render::Load(pugi::xml_node& node)
{
	// load the camera pos: 
	camera.x = node.child("cameraPos").attribute("x").as_uint();
	camera.y = node.child("cameraPos").attribute("y").as_uint();

	// load the scroll state: 
	auto scrollNode = node.child("scrollData"); 

	std::string scrollString = scrollNode.child("state").attribute("state").as_string();
	if (scrollString == "available")
		scrollState = AVAILABLE; 
	else if (scrollString == "scrolling")
	{

		scrollValues.speed = scrollNode.child("state").attribute("speed").as_int();
		scrollValues.worldDistance = scrollNode.child("state").attribute("worldDistance").as_float();
		scrollValues.originPos = scrollNode.child("state").attribute("originPos").as_int();

		scrollState = SCROLLING;
	}
		

	return true;
}

bool j1Render::Save(pugi::xml_node& node) const
{
	// save the camera pos: 
	auto pos = node.append_child("cameraPos");
	pos.append_attribute("x") = camera.x;
	pos.append_attribute("y") = camera.y;

	// save the state: it can be scrolling
	auto scrollNode = node.append_child("scrollData");

	auto scState = scrollNode.append_child("state");
	switch (scrollState)
	{
	case AVAILABLE:
		scState.append_attribute("state") = "available";
		break;
	case SCROLLING:
		scState.append_attribute("state") = "scrolling";
		break;
	default:
		break;
	}

	scState.append_attribute("speed") = scrollValues.speed;
	scState.append_attribute("worldDistance") = scrollValues.worldDistance;
	scState.append_attribute("originPos") = scrollValues.originPos;
   
	return true;
}
