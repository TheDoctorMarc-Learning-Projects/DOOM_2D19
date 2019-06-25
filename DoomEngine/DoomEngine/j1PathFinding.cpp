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
#
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
	/*if (App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		debug = !debug;

	if (debug)
		DebugDraw();

	iPoint pos = { 0,0 };
	// Draw Start Point
	pos = App->map->MapToWorld(origin.x, origin.y);
	App->render->DrawQuad({ pos.x,pos.y,App->map->data.tile_width, App->map->data.tile_height }, 0, 0, 255, 255);

	// Draw Goal
	pos = App->map->MapToWorld(goal.x, goal.y);
	App->render->DrawQuad({ pos.x,pos.y,App->map->data.tile_width, App->map->data.tile_height }, 0, 0, 255, 255);

	static char title[120];
	sprintf_s(title, 120, " Closed Nodes: %i, Open Nodes: %i, Visited Nodes: %i", closed.pathNodeList.size(), open.pathNodeList.size(), visited.pathNodeList.size());

	App->win->AddStringToTitle(title);*/

	// We draw the grid in front of all the Quads
//	DrawGrid();

	return true;
}

void j1PathFinding::DebugDraw()
{
	iPoint pos = { 0,0 };

	std::list<PathNode>::iterator iterator = visited.pathNodeList.begin();

	// Draw Visited Queue First (it stays on the bottom layer)
	for (iterator; iterator != visited.pathNodeList.end(); iterator++)
	{
		pos = App->map->MapToWorld((*iterator).pos.x, (*iterator).pos.y);
		App->render->DrawQuad({ pos.x,pos.y,App->map->data.tile_width, App->map->data.tile_height }, 150, 50, 255, 100);
	}

	// Draw Open Queue

	iterator = open.pathNodeList.begin();
	for (iterator; iterator != open.pathNodeList.end(); iterator++)
	{
		pos = App->map->MapToWorld((*iterator).pos.x, (*iterator).pos.y);
		App->render->DrawQuad({ pos.x,pos.y,App->map->data.tile_width, App->map->data.tile_height }, 0, 255, 0, 255);
	}
	// Draw Closed Queue

	iterator = closed.pathNodeList.begin();
	for (iterator; iterator != closed.pathNodeList.end(); iterator++)
	{
		pos = App->map->MapToWorld((*iterator).pos.x, (*iterator).pos.y);
		App->render->DrawQuad({ pos.x,pos.y,App->map->data.tile_width, App->map->data.tile_height }, 255, 0, 0, 255);
	}



}

void j1PathFinding::DrawGrid()
{
	// Horizontal	
	for (int x = 0; x < App->map->data.height + 1; ++x)
	{
		iPoint startPoint = App->map->MapToWorld(0, x);
		iPoint finalPoint = App->map->MapToWorld(App->map->data.width, x);
		App->render->DrawLine(startPoint.x, startPoint.y, finalPoint.x, finalPoint.y, 0, 0, 0);
	}

	// Vertical
	for (int j = 1; j < App->map->data.width + 1; ++j)
	{
		iPoint startPoint = App->map->MapToWorld(j, 0);
		iPoint finalPoint = App->map->MapToWorld(j, App->map->data.height);
		App->render->DrawLine(startPoint.x, startPoint.y, finalPoint.x, finalPoint.y, 0, 0, 0);
	}
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
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
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

const PathNode * PathList::FindJPS(const iPoint & point, const iPoint& direction) const
{
	std::list<PathNode>::const_iterator item = pathNodeList.begin();

	while (item != pathNodeList.end())
	{
		if ((*item).pos == point && (*item).direction == direction)
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
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;

	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this));

	// north-west
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// south-west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// north-west
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.pathNodeList.push_back(PathNode(-1, -1, cell, this, { 0,0 }, true));

	// south-est
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
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

int PathNode::CalculateF_JPS(const iPoint & destination)
{
	g = parent->g + parent->pos.DistanceManhattan(destination);

	h = pos.DistanceManhattan(destination);

	return g + h;
}

void j1PathFinding::CreatePath(const iPoint & origin, const iPoint & destination, bool usingJPS, bool stepByStep)
{
	if (usingJPS)
	{
		if (!stepByStep)
			CreatePathJPS(origin, destination);
		else
			StartJPS(origin, destination);
	}

	else
	{
		if (!stepByStep)
			CreatePathAStar(origin, destination);
		else
			StartAStar(origin, destination);
	}
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePathAStar(const iPoint& origin, const iPoint& destination)
{
	closed.pathNodeList.clear();
	open.pathNodeList.clear();
	visited.pathNodeList.clear();

	this->origin = origin;
	goal = destination;

	int ret = 0;
	last_path.clear();

	// If origin or destination are not walkable leave
	if (!IsWalkable(origin) || !IsWalkable(destination))
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

		/*	if (ret != -1)
			{
				for (uint i = 0; i < ret; ++i)
				{
					LOG("x = %i, y = %i", last_path[i].x, last_path[i].y);
				}
			}*/

			return ret;

		}
		else {
			// Fill a list of all adjancent nodes
			PathList neighbors;
			closed.pathNodeList.back().FindWalkableAdjacents(neighbors);

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

int j1PathFinding::CreatePathJPS(const iPoint & origin, const iPoint & destination)
{
	closed.pathNodeList.clear();
	open.pathNodeList.clear();
	visited.pathNodeList.clear();
	last_path.clear();

	goal = destination;
	this->origin = origin;

	if (!IsWalkable(origin) || !IsWalkable(goal))
	{
		return -1;
	}

	// TODO 7: Make the starting process and the cycle process of the JPS in one take here
	// It's really easy, its copying the same code you've in the correct position.  

	// Horizontal Cases

	// East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1,0 }));
	// West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1,0 }));

	// VERTICAL CASES 
	// North
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 0, 1 }));
	// South
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 0, -1 }));

	// DIAGONAL CASES 
	// North - East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1, 1 }));
	// South - East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1, -1 }));
	// South - West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1, -1 }));
	// North - West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1, 1 }));

	while (open.pathNodeList.empty() == false)
	{

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
		if (closed.pathNodeList.back().pos == goal) {

			for (PathNode iterator = closed.pathNodeList.back(); iterator.parent != nullptr;
				iterator = *closed.Find(iterator.parent->pos)) {

				last_path.push_back(iterator.pos);
			}

			last_path.push_back(closed.pathNodeList.front().pos);

			// Flip the path 
			std::reverse(last_path.begin(), last_path.end());

			return last_path.size();
		}
		else {
			// Fill a list of all adjancent nodes
			PathList neighbors;

			PruneAdjacents(closed.pathNodeList.back(), neighbors, &closed.pathNodeList.back());

			// Iterate adjancent nodes:
			std::list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

			while (iterator != neighbors.pathNodeList.end()) {
				// ignore nodes in the closed list
				if (closed.Find((*iterator).pos) != nullptr) {
					iterator++;
					continue;
				}

				(*iterator).CalculateF_JPS(goal);
				// If it is already in the open list, check if it is a better path (compare G)
				if (open.FindJPS((*iterator).pos, (*iterator).direction) != nullptr) {

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
	return -1;
}

void j1PathFinding::CyclePathfinding(bool usingJps)
{
	if (doingPath)
	{
		if (usingJps)
			CycleJPS();
		else CycleAStar();
	}

}


PathState j1PathFinding::StartAStar(const iPoint & origin, const iPoint & destination)
{
	closed.pathNodeList.clear();
	open.pathNodeList.clear();
	visited.pathNodeList.clear();
	last_path.clear();

	goal = destination;
	this->origin = origin;


	if (!IsWalkable(origin) || !IsWalkable(goal))
	{
		return PathState::Unavailable;
	}


	PathNode originNode(0, origin.DistanceTo(goal), origin, nullptr);
	open.pathNodeList.push_back(originNode);



	doingPath = true; 	// Debug purposes: asked when we want to do next step, so we dont overcommit
	return PathState::MAX;
}


PathState j1PathFinding::CycleAStar()
{
	if (open.pathNodeList.empty() == false)
	{
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
		if (closed.pathNodeList.back().pos == goal) {

			for (PathNode iterator = closed.pathNodeList.back(); iterator.parent != nullptr;
				iterator = *closed.Find(iterator.parent->pos)) {

				last_path.push_back(iterator.pos);
			}

			last_path.push_back(closed.pathNodeList.front().pos);

			// Flip the path
			std::reverse(last_path.begin(), last_path.end());
			doingPath = false;
			return PathState::Found;

		}
		else {
			// Fill a list of all adjancent nodes
			PathList neighbors;
			closed.pathNodeList.back().FindWalkableAdjacents(neighbors);

			// Iterate adjancent nodes:
			std::list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

			while (iterator != neighbors.pathNodeList.end()) {
				// ignore nodes in the closed list
				if (closed.Find((*iterator).pos) != NULL) {
					iterator++;
					continue;
				}

				(*iterator).CalculateF(goal);
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

	return PathState::Searching;
}

PathState j1PathFinding::StartJPS(const iPoint & origin, const iPoint & destination)
{
	closed.pathNodeList.clear();
	open.pathNodeList.clear();
	visited.pathNodeList.clear();
	last_path.clear();

	goal = destination;
	this->origin = origin;

	if (!IsWalkable(origin) || !IsWalkable(goal))
	{
		return PathState::Unavailable;
	}

	// TODO 0: Add 8 different starting nodes, with the 8 possible directions to the open list
	// DIAGONAL CASES 
	// North - East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1, 1 }));
	// South - East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1, -1 }));
	// South - West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1, -1 }));
	// North - West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1, 1 }));

	// VERTICAL CASES 
	// North
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 0, 1 }));
	// South
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 0, -1 }));

	// HORIZONTAL CASES 
	// East
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { 1,0 }));
	// West
	open.pathNodeList.push_back(PathNode(0, origin.DistanceManhattan(goal), origin, nullptr, { -1,0 }));


	doingPath = true; 	// Debug purposes: asked when we want to do next step, so we dont overcommit
	return PathState::MAX;
}

PathState j1PathFinding::CycleJPS()
{
	if (open.pathNodeList.empty() == false)
	{

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
		if (closed.pathNodeList.back().pos == goal) {

			for (PathNode iterator = closed.pathNodeList.back(); iterator.parent != nullptr;
				iterator = *closed.Find(iterator.parent->pos)) {

				last_path.push_back(iterator.pos);
			}

			last_path.push_back(closed.pathNodeList.front().pos);

			// Flip the path 
			std::reverse(last_path.begin(), last_path.end());
			doingPath = false;
			return PathState::Found;

		}
		else {
			// Fill a list of all adjancent nodes
			PathList neighbors;

			// TODO 1: Fill the nieghbours list with the pruned neighbours.  
			// Keep in mind that we do the same like in A*, only that we prune before adding elements
			// It's a single line

			PruneAdjacents(closed.pathNodeList.back(), neighbors, &closed.pathNodeList.back());

			// Iterate adjancent nodes:
			std::list<PathNode>::iterator iterator = neighbors.pathNodeList.begin();

			while (iterator != neighbors.pathNodeList.end()) {
				// ignore nodes in the closed list
				if (closed.Find((*iterator).pos) != nullptr) {
					iterator++;
					continue;
				}

				(*iterator).CalculateF_JPS(goal);
				// If it is already in the open list, check if it is a better path (compare G)
				if (open.FindJPS((*iterator).pos, (*iterator).direction) != nullptr) {

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
	return PathState::MAX;
}

void j1PathFinding::PruneAdjacents(const PathNode & node, PathList & listToFill, const PathNode* parent)
{
	if (node.direction.y == 0)
		HorizontalJump(node, listToFill, parent);
	else if (node.direction.x == 0)
		VerticalJump(node, listToFill, parent);
	else DiagonalJump(node, listToFill, parent);
}

void j1PathFinding::HorizontalJump(const PathNode& node, PathList& listToFill, const PathNode* parent)
{
	iPoint newPos = node.pos + node.direction;
	PathNode jumpPoint(-1, -1, newPos, parent, { 0,0 });

	int horizontalDir = node.direction.x;

	if (!IsWalkable(newPos))
	{
		return;
	}

	if (newPos == goal)
	{
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}

	// Only for debugging
	visited.pathNodeList.push_back(jumpPoint);
	// --- 

	// TODO 2: Find any possible forced neighbour for an horizontal Jump
	// When we find one, we have to add it to the list to make sure its analyzed later on (with the proper direction), and we exit
	// Also, before we exit, don't forget to add the current propagation (as a node), so it can be completed and not forgotten
	if (!IsWalkable(newPos + iPoint(0, 1)) && IsWalkable(newPos + iPoint(horizontalDir, 1)) && IsWalkable(newPos + node.direction))
	{
		jumpPoint.direction = { horizontalDir, 1 };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (!IsWalkable(newPos + iPoint(0, -1)) && IsWalkable(newPos + iPoint(horizontalDir, -1)) && IsWalkable(newPos + node.direction))
	{
		jumpPoint.direction = { horizontalDir, -1 };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (listToFill.pathNodeList.empty() == false)
	{
		jumpPoint.direction = node.direction;
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}

	// TODO 3: Really simple: if not found any forced neighbour, we just keep on jumping in the same direction as we are "coming from"
	// You have to make sure to return what that jump returns us
	jumpPoint.direction = node.direction;
	return HorizontalJump(jumpPoint, listToFill, parent);
}

void j1PathFinding::VerticalJump(const PathNode & node, PathList& listToFill, const PathNode* parent)
{
	iPoint newPos = node.pos + node.direction;
	PathNode jumpPoint(-1, -1, newPos, parent, { 0,0 });

	int verticalDir = node.direction.y;

	if (!IsWalkable(newPos))
	{
		return;
	}

	if (newPos == goal)
	{
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}

	// Only for debugging
	visited.pathNodeList.push_back(jumpPoint);
	// --- 

	if (!IsWalkable(newPos + iPoint(1, 0)) && IsWalkable(newPos + iPoint(1, verticalDir)) && IsWalkable(newPos + node.direction))
	{
		jumpPoint.direction = { 1, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (!IsWalkable(newPos + iPoint(-1, 0)) && IsWalkable(newPos + iPoint(-1, verticalDir)) && IsWalkable(newPos + node.direction))
	{
		jumpPoint.direction = { -1, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (listToFill.pathNodeList.empty() == false)
	{
		jumpPoint.direction = node.direction;
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}

	jumpPoint.direction = node.direction;
	return VerticalJump(jumpPoint, listToFill, parent);
}

void j1PathFinding::DiagonalJump(const PathNode & node, PathList& listToFill, const PathNode* parent)
{
	iPoint newPos = node.pos + node.direction;
	PathNode jumpPoint(-1, -1, newPos, parent, node.direction);

	int horizontalDir = node.direction.x;
	int verticalDir = node.direction.y;

	if (!IsWalkable(newPos))
		return;

	if (newPos == goal)
	{
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}


	// Only for debugging
	visited.pathNodeList.push_back(jumpPoint);
	// --- 


	if (!IsWalkable(newPos + iPoint(-horizontalDir, 0)) && IsWalkable(newPos + iPoint(-horizontalDir, verticalDir))
		&& IsWalkable(newPos + iPoint(0, verticalDir)))
	{
		jumpPoint.direction = { -horizontalDir, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);

		// Extra cases? 
		jumpPoint.direction = { horizontalDir, 0 };
		listToFill.pathNodeList.push_back(jumpPoint);

		jumpPoint.direction = { 0, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (!IsWalkable(newPos + iPoint(0, -verticalDir)) && IsWalkable(newPos + iPoint(horizontalDir, -verticalDir))
		&& IsWalkable(newPos + iPoint(horizontalDir, 0)))
	{
		jumpPoint.direction = { horizontalDir, -verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);

		// Extra cases? 
		jumpPoint.direction = { horizontalDir, 0 };
		listToFill.pathNodeList.push_back(jumpPoint);

		jumpPoint.direction = { 0, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);
	}

	if (listToFill.pathNodeList.empty() == false)
	{
		jumpPoint.direction = node.direction;
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}


	// TODO 4: Make an horizontal jump with the proper direction
	// If it detected any forced neighbour, you have to add 
	// to the list the remaining directions that we are looking for (as nodes)
	// so they don't get lost. 
	jumpPoint.direction = { horizontalDir, 0 };
	HorizontalJump(jumpPoint, listToFill, parent);
	if (listToFill.pathNodeList.empty() == false)
	{
		jumpPoint.direction = node.direction;
		listToFill.pathNodeList.push_back(jumpPoint);

		jumpPoint.direction = { 0, verticalDir };
		listToFill.pathNodeList.push_back(jumpPoint);
		return;
	}
	else
	{
		// TODO 5: Make a vertical Jump with the proper direction if the Horizontal jump
		// did not find any jumpPoints. Like before, make sure to add the remaining directions.
		jumpPoint.direction = { 0, verticalDir };
		VerticalJump(jumpPoint, listToFill, parent);
		if (listToFill.pathNodeList.empty() == false)
		{
			jumpPoint.direction = node.direction;
			listToFill.pathNodeList.push_back(jumpPoint);

			jumpPoint.direction = { horizontalDir, 0 };
			listToFill.pathNodeList.push_back(jumpPoint);
			return;
		}
		else
		{
			// TODO 6: If none of the past two jumps returned an interesting Point, make a diagonal Jump
			// with the same direction as this one.
			jumpPoint.direction = node.direction;
			return DiagonalJump(jumpPoint, listToFill, parent);
		}
	}


}

