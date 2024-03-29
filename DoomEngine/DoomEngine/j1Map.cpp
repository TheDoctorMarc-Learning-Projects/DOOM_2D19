#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Window.h"
#include "j1Input.h"
#include "j1EntityFactory.h"
#include "LootWeaponMaps.h"

#include "Brofiler/Brofiler.h"

#include <math.h>


j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.assign("map");
}

// Destructor
j1Map::~j1Map()
{
	for (auto tex : entityTextureMap)
	{
		App->tex->UnLoad(tex.second);
		tex.second = nullptr;
	}
	entityTextureMap.clear();

}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.assign(config.child("folder").child_value());

	// configures the pixel offset (center top up isometric corner to real 0,0 world coord
	// if not, all the rest are 1 tile displaced
	// for this, the worldToMap function on x needs to be workarounded by substracting -1
	//pixelTileOffset.create(0,-16);//-64 * 0.5f, -32 * 0.5f);

	return ret;
}

bool j1Map::Start()
{

	// object textures
	entityTextureMap.insert(std::make_pair("loot", App->tex->Load("textures/loot/loot.png")));
	entityTextureMap.insert(std::make_pair("platform1", App->tex->Load("maps/textures/plat1.png")));
	entityTextureMap.insert(std::make_pair("platform2", App->tex->Load("maps/textures/plat2.png")));

	return true; 
}

void j1Map::Draw()
{
	BROFILER_CATEGORY("Map Draw", Profiler::Color::DeepPink);

	if (map_loaded == false)
		return;

	std::list<MapLayer*>::iterator layer = data.layers.begin();
	
	for (auto& layer : data.layers)
	{
		for (int i = 0; i < data.width; ++i)
		{
			for (int j = 0; j < data.height; ++j)
			{
				
					int tile_id = layer->Get(i, j);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);
						if (tileset != nullptr)
						{

							SDL_Rect r = tileset->GetTileRect(tile_id);
							iPoint pos = MapToWorld(i, j);

							App->render->Blit(tileset->texture, pos.x - data.tile_width, pos.y - data.tile_height, &r, layer->properties.parallaxSpeed);
						}

					}
			
			}

		}

	}
		


}

int Properties::Get(const char* value, int default_value) const
{
	std::list<Property*>::const_iterator item;
	item = list.begin();

	while(item != list.end())
	{
		if((*item)->name == value)
			return (*item)->value;
		++item;
	}

	return default_value;
}

TileSet* j1Map::GetTilesetFromTileId(int id) const
{
	std::list<TileSet*>:: const_iterator item = data.tilesets.begin();
	TileSet* set = *item;

	while(item != data.tilesets.end())
	{
		if(id < (*item)->firstgid)
		{
			set = *(--item);
			++item;
			break;
		}
		set = *item;
		++item;
	}

	return set;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	if (num_tiles_width != 0)
	{
		rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
		rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	}
	return rect;
}


iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);

	if(data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}

	return ret;
}


// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	std::list<TileSet*>::iterator tiles_item;
	tiles_item = data.tilesets.begin();

	while(tiles_item != data.tilesets.end())
	{
		if ((*tiles_item) != nullptr && (*tiles_item)->texture != nullptr)
		{
			App->tex->UnLoad((*tiles_item)->texture);
			(*tiles_item)->texture = nullptr;
		}
		delete *tiles_item;
		*tiles_item = nullptr;
		++tiles_item;
	}
	data.tilesets.clear();

	// Remove all layers
	std::list<MapLayer*>::iterator layer_item;
	layer_item = data.layers.begin();

	while(layer_item != data.layers.end())
	{
		// testing 

		RELEASE((*layer_item)->data); 



	//	data.layers.remove(*layer_item)

		delete *layer_item;
		*layer_item = nullptr;
		++layer_item;
	}
	data.layers.clear();



	map_loaded = false;
	// Clean up the pugui tree
	map_file.reset();
	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{

	bool ret = true;
	pugi::xml_parse_result result = map_file.load_file(file_name);

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = DBG_NEW TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = DBG_NEW MapLayer();

		ret = LoadLayer(layer, lay);

		if (ret == true)
			data.layers.push_back(lay);
	}

/*	pugi::xml_node objectlayer;
	for (objectlayer = map_file.child("map").child("objectgroup"); objectlayer && ret; objectlayer = objectlayer.next_sibling("objectgroup"))
	{
		std::string tmp(objectlayer.attribute("name").as_string());
		if (tmp == "Spawns")
		{
			//LoadSpawns(objectlayer);
		}
	}*/

	// Load objects/scene colliders -----------------------------------------
	pugi::xml_node objectGroup;
	for (objectGroup = map_file.child("map").child("objectgroup"); objectGroup && ret; objectGroup = objectGroup.next_sibling("objectgroup"))
	{
		std::string tmp(objectGroup.attribute("name").as_string());
		
		LoadMapObjects(objectGroup); 
	
	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		std::list<TileSet*>::iterator item = data.tilesets.begin();
		while (item != data.tilesets.end())
		{
			TileSet* s = (*item);
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.data(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			++item;
		}

		std::list<MapLayer*>::iterator item_layer = data.layers.begin();
		while (item_layer != data.layers.end())
		{
			MapLayer* l = (*item_layer);
			LOG("Layer ----");
			LOG("name: %s", l->name.data());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			++item_layer;
		}
	}

	map_loaded = ret;

	return ret;
}



// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if(map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if(bg_color.length() > 0)
		{
			std::string red, green, blue;
			red = (1, 2);
			green = (3, 4);
			blue = (5, 6);

			int v = 0;

			sscanf_s(red.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.data(), "%x", &v);
			if(v >= 0 && v <= 255) data.background_color.b = v;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if(orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if(orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if(orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
		// load custom properties from map data
		/*pugi::xml_node propertiesNode = map.child("properties");

		if (propertiesNode == NULL)
		{
			LOG("Map without custom properties");
		}
		else
		{
			LoadProperties(map, data.properties);
		}*/
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.assign(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();

	pugi::xml_node offset = tileset_node.child("tileoffset");
	if(offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if(image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		const char* pathPrefix = "maps"; 
		const char* pathDataComplete = PATH(pathPrefix, image.attribute("source").as_string()); 

		set->texture = App->tex->Load(pathDataComplete);
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if(set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if(set->tex_height <= 0)
		{
			set->tex_height = h;
		}

			set->num_tiles_width = set->tex_width / set->tile_width;
			set->num_tiles_height = set->tex_height / set->tile_height; 	
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer);
	

	pugi::xml_node layer_data = node.child("data");


	iPoint layer_size;
	

	layer_size.x = (layer->width + layer->height)*(App->map->data.tile_width *0.5f);
	layer_size.y = (layer->width + layer->height + 1) * (data.tile_height *0.5f);



	//TEST


	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = DBG_NEW uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			int value = tile.attribute("gid").as_int();
			layer->data[i++] = value;
		}
	}

	return ret;
}


bool j1Map::LoadMapObjects(pugi::xml_node& node)
{
	bool ret = true;

	for (auto object = node.child("object"); object && ret; object = object.next_sibling("object"))
	{
		SDL_Rect worldPos;
		worldPos.x = object.attribute("x").as_int() - data.tile_width;
		worldPos.y = object.attribute("y").as_int() - data.tile_height;
		worldPos.w = object.attribute("width").as_int();
		worldPos.h = object.attribute("height").as_int();

		std::string ObjectName = object.attribute("name").as_string();

	 

		if (ObjectName == "floor")
		{
			
			App->collision->AddCollider(worldPos, COLLIDER_TYPE::COLLIDER_FLOOR); 
		}

		else if (ObjectName == "wall")
		{

			App->collision->AddCollider(worldPos, COLLIDER_TYPE::COLLIDER_WALL);
			for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
			{
				std::string name = property.attribute("name").as_string();
				if (name == "mapLimitX")
					mapLimitXWorldPos = worldPos.x; 
			}

		}

		else if (ObjectName == "staticPlatform")
		{
			int heighLevel = 0; 

			for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
			{
				std::string name = property.attribute("name").as_string();
				if (name == "heightLevel")
				{
					heighLevel = property.attribute("value").as_int();
					App->entityFactory->platFormLevelHeights[heighLevel] = worldPos.y;
				}
		
				

			}

			

			App->entityFactory->CreatePlatform(ENTITY_TYPE::ENTITY_STATIC, worldPos, heighLevel, "platform");
		}

		else if (ObjectName == "dynamicPlatform")
		{
			int heightLevel = 0; 
			std::string Axis = ""; 
			int levelsUp = 0;
			int levelsDown = 0; 

			for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
			{
				std::string name = property.attribute("name").as_string(); 
				if (name == "heightLevel")
					heightLevel = property.attribute("value").as_int();
				else if(name == "axis")
					Axis = property.attribute("value").as_string();
				else if (name == "levelsUp")
					levelsUp = property.attribute("value").as_int();
				else if (name == "levelsDown")
					levelsDown = property.attribute("value").as_int();
			}

			AXIS_Movement axisMov; 
			
			if (Axis == "horizontal")
				axisMov = AXIS_Movement::HORIZONTAL;
			else if(Axis == "vertical")
			{
				axisMov = AXIS_Movement::VERTICAL;
			}
				

			App->entityFactory->CreatePlatform(ENTITY_TYPE::ENTITY_DYNAMIC, worldPos, heightLevel, "platform",
				levelsUp, levelsDown, App->scene->GetNextSceneState(), axisMov);

		}

		else 
		{
			// weapons, health, ammo:: 

			if (ObjectName == "weapon")
			{
				std::string weaponName;
				std::string firingTypeName;

				weaponInfo weaponData;                // capture weapon data 

				for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
				{
					std::string name = property.attribute("name").as_string();
					if (name == "name")
						weaponName = property.attribute("value").as_string();
					else if (name == "firingType")
						firingTypeName = property.attribute("value").as_string();
					else if (name == "damage")
						weaponData.damage = property.attribute("value").as_float();
					else if (name == "cadence")
						weaponData.cadence = property.attribute("value").as_int();
					else if (name == "maxBullets")
						weaponData.maxBullets = property.attribute("value").as_int();
					else if (name == "scope")
						weaponData.scope = property.attribute("value").as_float();


				}
				weaponData.weaponType = weaponTypeMap.at(weaponName);
				weaponData.FiringType = weaponFiringTypeMap.at(firingTypeName);


				App->entityFactory->CreateWeapon(ENTITY_TYPE::LOOT, worldPos.x, worldPos.y, weaponName, LOOT_TYPE::WEAPON, weaponData);
			}


			else if (ObjectName == "figure")
			{
				bool classic = false;

				for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
				{
					std::string name = property.attribute("name").as_string();
					if (name == "classicType")
						classic = property.attribute("value").as_bool();



				}

				std::string UIName = "oldCollectible";

				if (classic == false)
					UIName = "newCollectible";


				App->entityFactory->CreateCoin(ENTITY_TYPE::LOOT, worldPos.x, worldPos.y, UIName, LOOT_TYPE::COIN, classic);
			}


			else if (ObjectName == "health" || ObjectName == "ammo" || ObjectName == "armor")
			{

				if (ObjectName == "health")
					App->entityFactory->CreateHealth(ENTITY_TYPE::LOOT, worldPos.x, worldPos.y, "health", LOOT_TYPE::HEALTH);
				else if (ObjectName == "ammo")
					App->entityFactory->CreateAmmo(ENTITY_TYPE::LOOT, worldPos.x, worldPos.y, "ammo", LOOT_TYPE::AMMO);
				else if (ObjectName == "armor")
					App->entityFactory->CreateArmor(ENTITY_TYPE::LOOT, worldPos.x, worldPos.y, "armor", LOOT_TYPE::ARMOR);

			}


			else if (ObjectName == "enemy")
			{
				std::string enemyName = "";

				for (auto property = object.child("properties").child("property"); property; property = property.next_sibling("property"))
				{
					std::string propertyName = property.attribute("name").as_string();
					if (propertyName == "type")
						enemyName = property.attribute("value").as_string();;

				}


				encapsulatedEnemyData* enemyData = DBG_NEW encapsulatedEnemyData(iPoint(worldPos.x, worldPos.y), enemyName);
				App->entityFactory->enemiesToBeSpawned.push_back(enemyData);




			}

			else if (ObjectName == "player")
				App->entityFactory->playerSpawnPos = iPoint(worldPos.x, worldPos.y);

			else if (ObjectName == "win")
				App->collision->AddCollider(worldPos, COLLIDER_WIN, nullptr); 


		}

	}

	return ret; 
	
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = false;

	pugi::xml_node propertiesNode = node.child("properties");

	if (propertiesNode != NULL)
	{
		pugi::xml_node prop;

		for (prop = propertiesNode.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = DBG_NEW Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_float();
			
			if(p->name == "parallaxSpeed")
				layer->properties.parallaxSpeed = node.child("properties").child("property").attribute("value").as_float();
	

			layer->properties.list.push_back(p);
		}
	}

	return ret;
}


bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer) const
{
	bool ret = false;
	std::list<MapLayer*>:: const_iterator item;   
	item = data.layers.begin();

	for(; *item != NULL; ++item)
	{
		MapLayer* layer = *item;

		if(layer->properties.Get("Navigation", 0) == 0)
			continue;
		else
		{
			uchar* map = DBG_NEW uchar[layer->width*layer->height];
			memset(map, 0, layer->width*layer->height);

			for (int y = 0; y < data.height; ++y)
			{
				for (int x = 0; x < data.width; ++x)
				{
					int i = (y*layer->width) + x;

					int tile_id = layer->Get(x, y);
					TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

					if (tileset != NULL)
					{
						int value = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
						map[i] = value; 

					}
				}
			}

			*buffer = map;
			width = data.width;
			height = data.height;
			ret = true;

			break;
		}

	
	}

	return ret;
}

