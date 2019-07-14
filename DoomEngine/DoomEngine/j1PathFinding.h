#ifndef __j1PATHFINDING_H__
#define __j1PATHFINDING_H__

#include "j1Module.h"
#include "p2Point.h"

#include <list>
#include <vector>
#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255

// --------------------------------------------------
// Recommended reading:
// Intro: http://www.raywenderlich.com/4946/introduction-to-a-pathfinding
// Details: http://theory.stanford.edu/~amitp/GameProgramming/
// --------------------------------------------------

enum class PathState {
	Found,
	Searching,
	Unavailable,
	MAX
};

enum class Direction {
	N,
	NE,
	E,
	SE,
	S,
	SW,
	W,
	NW,
	NONE
};

// forward declaration
struct PathList;
struct SDL_Texture;
// ---------------------------------------------------------------------
// Pathnode: Helper struct to represent a node in the path creation
// ---------------------------------------------------------------------
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent, const iPoint& direction = iPoint(0, 0), bool isDiagonal = false);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill, bool walkableAccounts = true) const;

	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);
	int CalculateF_JPS(const iPoint& destination);

	// -----------
	int g;
	int h;
	iPoint pos;
	iPoint direction;
	const PathNode* parent; // needed to reconstruct the path in the end
	bool isDiagonal;
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{
	// Looks for a node in this list and returns it's list node or NULL
	const PathNode* Find(const iPoint& point) const;
	const PathNode* FindJPS(const iPoint& point, const iPoint& direction) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	const PathNode* GetNodeLowestScore() const;

	// -----------
	// The list itself, note they are not pointers!
	std::list<PathNode> pathNodeList;
};

class j1PathFinding : public j1Module
{
public:

	j1PathFinding();

	// Destructor
	~j1PathFinding();

	bool Start() override;

	bool PostUpdate() override;

	void DebugDraw();
	void DrawGrid();
	// Called before quitting
	bool CleanUp();

	// Choses algorithm to work with
	void CreatePath(const iPoint& origin, const iPoint& destination, bool usingJps = true, bool stepByStep = false);

	// Main function to request a path from A to B
	int CreatePathAStar(const iPoint& origin, const iPoint& destination, bool walkableAccounts = true);
	int CreatePathJPS(const iPoint& origin, const iPoint& destination);

	// Main function to cycle the algorythm, choses which one
	void CyclePathfinding(bool usingJps);

	// Step by step methods
	PathState StartAStar(const iPoint& origin, const iPoint& destination);
	PathState CycleAStar();

	PathState StartJPS(const iPoint& origin, const iPoint& destination);
	PathState CycleJPS();


	// JPS functionality
	void HorizontalJump(const PathNode& node, PathList& listToFill, const PathNode* parent);
	void VerticalJump(const PathNode& node, PathList& listToFill, const PathNode* parent);
	void DiagonalJump(const PathNode& node, PathList& listToFill, const PathNode* parent);

	void PruneAdjacents(const PathNode& node, PathList& listToFill, const PathNode* parent);

	// To request all tiles involved in the last generated path
	const std::vector<iPoint>* GetLastPath() const;


	// Walkability Map Functions

	// Sets up the walkability map
	void SetMap(uint width, uint height, uchar* data);

	// Utility: return true if pos is inside the map boundaries
	bool CheckBoundaries(const iPoint& pos) const;

	// Utility: returns true is the tile is walkable
	bool IsWalkable(const iPoint& pos) const;

	// Utility: return the walkability value of a tile
	uchar GetTileAt(const iPoint& pos) const;

	// Returns index from walkability Map from a Tile Pos on map
	uint GetIndexAt(const iPoint& pos) const;

	// Returns the Tile from walkability Map from ID
	iPoint GetTileFromWalkability(int id) const;

	// Returns the World Position from walkability Map from ID
	iPoint GetPosFromWalkability(int id) const;

	// Swaps Walkability Map Value
	void ActivateTile(const iPoint& tile);
	void DeactivateTile(const iPoint& tile);

	bool debug = false;
	bool doingPath = false;

public:
	uchar* map = nullptr;

private:

	PathList open;
	PathList closed;
	PathList visited;

	// size of the map
	uint width;
	uint height;
	// all map walkability values [0..255]
	// we store the created path here
	std::vector<iPoint> last_path;

	iPoint goal = { -1, -1 };
	iPoint origin = { -1, -1 };
	PathState state = PathState::MAX;

};


#endif // __j1PATHFINDING_H__