#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include <list>
#include "p2Point.h"
#include "j1Module.h"
#include "SDL/include/SDL.h"
#include <map>

struct SDL_Rect; 
struct SDL_Texture; 
struct SDL_Color; 

struct Properties
{
	struct Property
	{
		std::string name;
		int value;
	};

	~Properties()
	{
		std::list<Property*>:: iterator item;
		item = list.begin();

		while(item != list.end())
		{
			delete *item; 
			*item = nullptr; 
			++item;
		}

		list.clear();
	}





	float parallaxSpeed = 1.0f;

	int Get(const char* name, int default_value = 0) const;

	std::list<Property*>	list;
};


// ----------------------------------------------------
struct MapLayer
{
	std::string	name;
	int			width;
	int			height;
	uint*		data = nullptr;
	Properties	properties;
	


	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{


//		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x]; 
	}
};

// ----------------------------------------------------
struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	std::string			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture = nullptr;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int						width;
	int						height;
	int						tile_width;
	int						tile_height;
	SDL_Color				background_color;
	MapTypes				type;
	std::list<TileSet*>		tilesets;
	std::list<MapLayer*>	layers;
	Properties				properties;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();


	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;

	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

	TileSet* GetTilesetFromTileId(int id) const;
private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, MapLayer* layer);
    bool LoadMapObjects(pugi::xml_node& node);
	/*bool LoadSpawns(pugi::xml_node& node); */


public:
	MapData data;
	bool map_loaded;
	int mapLimitXWorldPos = 0; 
	bool mapDebug = false; 

	std::map<std::string, SDL_Texture*> entityTextureMap;




	//iPoint pixelTileOffset; // mesures the height between the end of y sprite texture to start of the paint(draw) itself (displacement)
private:

	pugi::xml_document	map_file;
	std::string			folder;
	bool debugDraw = false;
};

#endif // __j1MAP_H__