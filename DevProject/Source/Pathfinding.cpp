#include "Defs.h"
#include "Log.h"
#include "App.h"
#include "PathFinding.h"

PathFinding::PathFinding() : Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH), width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

void PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);

	if (t > 0 && t != INVALID_WALK_CODE)
		return true;
	else
		return false;


}

uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

const DynArray<iPoint>* PathFinding::GetLastPath() const
{
	return &last_path;
}

List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	List_item<PathNode>* item = list.start;
	while (item)
	{
		if (item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	List_item<PathNode>* ret = NULL;
	int min = 65535;

	List_item<PathNode>* item = list.end;
	while (item)
	{
		if (item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

Direction PathFinding::current_Direction(DynArray<iPoint>&path) const {

	if (path.Count() >= 2) {

		iPoint current_Tile =path[0];
		iPoint next_Tile =path[1];
		iPoint Direction_comp;

		Direction_comp.x = next_Tile.x - current_Tile.x;
		Direction_comp.y = next_Tile.y - current_Tile.y;

		if (Direction_comp.x == 1 && Direction_comp.y == 1)
			return DOWN_RIGHT;
		else if (Direction_comp.x == -1 && Direction_comp.y == 1)
			return DOWN_LEFT;
		else if (Direction_comp.x == 1 && Direction_comp.y == -1)
			return UP_RIGHT;
		else if (Direction_comp.x == 1 && Direction_comp.y == -1)
			return UP_LEFT;
		else if (Direction_comp.x == 1)
			return RIGHT;
		else if (Direction_comp.x == -1)
			return LEFT;
		else if (Direction_comp.y == 1)
			return DOWN;
		else if (Direction_comp.y == -1)
			return UP;

	}
	else return NONE;
}

PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;

	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

int PathNode::Score() const
{
	return g + h;
}

int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceManhattan(destination);

	return g + h;
}

DynArray<iPoint>* PathFinding::CreatePath(const iPoint& origin, const iPoint& destination) {

	if (!IsWalkable(origin) || !IsWalkable(destination))
		return nullptr;

	PathList open;
	PathList close;

	open.list.add(PathNode(0, 0, origin, NULL));

	while (open.list.count() > 0) {

		List_item<PathNode>* aux = open.GetNodeLowestScore();

		List_item<PathNode> *lower = close.list.add(aux->data);
		open.list.del(aux);

		if (lower->data.pos == destination) {

			last_path.Clear();

			const PathNode *new_node = &lower->data;
			int i = 0;

			while (new_node) {

				last_path.PushBack(new_node->pos);
				new_node = new_node->parent;
				LOG("    new_node: %i,%i", last_path.At(i)->x, last_path.At(i)->y);
				i++;
			}

			last_path.Flip();
			return &last_path;
		}

		PathList AdjacentNodes;
		AdjacentNodes.list.clear();
		lower->data.FindWalkableAdjacents(AdjacentNodes);

		List_item<PathNode> *i = AdjacentNodes.list.start; 
		for (; i; i = i->next) {

			if (close.Find(i->data.pos) != NULL)
				continue;

			List_item<PathNode> *adj_node = open.Find(i->data.pos);

			if (adj_node == NULL) {

				i->data.CalculateF(destination);
				open.list.add(i->data);
			}
			else if (adj_node->data.g > i->data.g + 1) {

				adj_node->data.parent = i->data.parent;
				adj_node->data.CalculateF(destination);

			}
		}
	}

	return nullptr;
}