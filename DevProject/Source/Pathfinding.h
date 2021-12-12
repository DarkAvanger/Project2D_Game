#ifndef __PATHFINDING_H__
#define __PATHFINDING_H__

#include "Module.h"
#include "Point.h"
#include "DynArray.h"

#define DEFAULT_PATH_LENGTH 50
#define INVALID_WALK_CODE 255


enum Direction {

	UP,
	DOWN,
	LEFT,
	RIGHT,
	UP_RIGHT,
	UP_LEFT,
	DOWN_RIGHT,
	DOWN_LEFT,
	NONE
	
};

class PathFinding : public Module
{
public:

	PathFinding();

	// Destructor
	~PathFinding();

	// Called before quitting
	bool CleanUp();

	void SetMap(uint width, uint height, uchar* data);

	DynArray<iPoint>* CreatePath(const iPoint& origin, const iPoint& destination);

	const DynArray<iPoint>* GetLastPath() const;

	bool CheckBoundaries(const iPoint& pos) const;

	bool IsWalkable(const iPoint& pos) const;

	uchar GetTileAt(const iPoint& pos) const;

	Direction current_Direction(DynArray<iPoint>&path)const;

private:

	uint width;
	uint height;
	uchar* map;
	DynArray<iPoint> last_path;

};

struct PathList;

struct PathNode
{
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	int Score() const;
	int CalculateF(const iPoint& destination);

	int g;
	int h;
	iPoint pos;
	const PathNode* parent; 
};

struct PathList
{
	List_item<PathNode>* Find(const iPoint& point) const;

	List_item<PathNode>* GetNodeLowestScore() const;

	List<PathNode> list;
};


#endif // __PATHFINDING_H__