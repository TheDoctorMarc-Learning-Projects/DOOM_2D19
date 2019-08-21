#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Render.h"
#include "j1Map.h"
#include "j1Textures.h"
#include "j1Timer.h"
#include "j1Input.h"
#include "j1Window.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(0), width(0), height(0)
{
	name.assign("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

bool j1PathFinding::Start()
{
	return true;
}

bool j1PathFinding::PostUpdate()
{
	if (App->pause == true)
		return true;

	return true;
}
// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 1 && pos.x <= (int)width - 1 &&
		pos.y >= 1 && pos.y <= (int)height - 1);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

uint j1PathFinding::GetIndexAt(const iPoint & pos) const
{
	return uint(pos.y * width + pos.x);
}

iPoint j1PathFinding::GetTileFromWalkability(int id) const
{
	return iPoint((id % width), (int)(id / width));
}

iPoint j1PathFinding::GetPosFromWalkability(int id) const
{
	int x = (id % width);
	int y = (int)(id / width);

	iPoint ret = App->map->MapToWorld(x, y);
	return ret;
}

void j1PathFinding::ActivateTile(const iPoint& tile)
{
	if (CheckBoundaries(tile))
	{
		int id = GetIndexAt(tile);
		if (map[id] != 0)
			map[id] = 0;
	}
}

void j1PathFinding::DeactivateTile(const iPoint& tile)
{
	if (CheckBoundaries(tile))
	{
		int id = GetIndexAt(tile);
		if (map[id] == 0)
			map[id] = 1;
	}
}

// To request all tiles involved in the last generated path
const std::vector<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
const PathNode* PathList::Find(const iPoint& point) const
{
	std::list<PathNode>::const_iterator item = pathNodeList.begin();

	while (item != pathNodeList.end())
	{
		if ((*item).pos == point)
			return &(*item);
		item++;
	}

	return nullptr;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
const PathNode* PathList::GetNodeLowestScore() const
{
	const PathNode* ret = nullptr;
	int min = INT_MAX;

	std::list<PathNode>::const_reverse_iterator item = pathNodeList.rbegin();

	while (item != pathNodeList.rend())
	{
		if ((*item).Score() < min)
		{
			min = (*item).Score();
			ret = &(*item);
		}
		item++;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(nullptr), direction(0, 0)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent, const iPoint& direction, bool isDiagonal) : g(g), h(h), pos(pos), parent(parent), direction(direction), isDiagonal(isDiagonal)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent), direction(node.direction)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill, bool walkableAccounts) const
{
	iPoint cell;

	cell.create(pos.x, pos.y + 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// north-west
	cell.create(pos.x + 1, pos.y - 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// south-west
	cell.create(pos.x - 1, pos.y - 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// north-west
	cell.create(pos.x + 1, pos.y + 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// south-est
	cell.create(pos.x - 1, pos.y + 1);
	if (!walkableAccounts || (walkableAccounts && App->pathfinding->IsWalkable(cell)))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	return list_to_fill.pathNodeList.size();
}


// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	if (isDiagonal)
		g = parent->g + 1.7f;
	else
		g = parent->g + 1.0f;

	h = pos.DistanceManhattan(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePathAStar(const iPoint& origin, const iPoint& destination, bool walkableAccounts)
{
	closed.pathNodeList.clear();
	open.pathNodeList.clear();
	visited.pathNodeList.clear();

	this->origin = origin;
	goal = destination;

	int ret = 0;
	last_path.clear();

	// If origin or destination are not walkable leave
	if (walkableAccounts && (!IsWalkable(origin) || !IsWalkable(destination)))
	{
		return -1;
	}

	PathNode originNode(0, origin.DistanceTo(destination), origin, nullptr);
	open.pathNodeList.push_back(originNode);

	while (open.pathNodeList.size() > 0) {

		// Move the lowest score cell from open list to the closed list
		PathNode* curr = (PathNode*)open.GetNodeLowestScore();
		closed.pathNodeList.push_back(*curr);

		// Erase element from list -----
		std::list<PathNode>::iterator it = open.pathNodeList.begin();
		while (it != open.pathNodeList.end()) {

			if (&(*it) == &(*curr))
				break;
			it++;
		}
		open.pathNodeList.erase(it);
		// Erase element from list -----

		// If we just added the destination, we are done!
		// Backtrack to create the final path
		if (closed.pathNodeList.back().pos == destination) {

			for (PathNode iterator = closed.pathNodeList.back(); iterator.parent != nullptr;
				iterator = *closed.Find(iterator.parent->pos)) {

				last_path.push_back(iterator.pos);
			}

			last_path.push_back(closed.pathNodeList.front().pos);

			// Flip() the path when you are finish
			std::reverse(last_path.begin(), last_path.end());

			ret = last_path.size();

			return ret;

		}
		else {
			// Fill a list of all adjancent nodes
			PathList neighbors;
			closed.pathNodeList.back().FindWalkableAdjacents(neighbors, walkableAccounts);

			// Iterate adjancent nodes:
			std::list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

			while (iterator != neighbors.pathNodeList.end()) {
				// ignore nodes in the closed list
				if (closed.Find((*iterator).pos) != NULL) {
					iterator++;
					continue;
				}

				(*iterator).CalculateF(destination);
				// If it is already in the open list, check if it is a better path (compare G)
				if (open.Find((*iterator).pos) != NULL) {

					// If it is a better path, Update the parent
					PathNode open_node = *open.Find((*iterator).pos);
					if ((*iterator).g < open_node.g)
						open_node.parent = (*iterator).parent;
				}
				else {
					// If it is NOT found, calculate its F and add it to the open list
					open.pathNodeList.push_back(*iterator);
				}
				iterator++;
			}
			neighbors.pathNodeList.clear();
		}
	}

	return ret;

}
