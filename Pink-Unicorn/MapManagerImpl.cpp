#include "MapManager.h"
#include <queue>
using namespace BWAPI;

MapManager * MapManager::insta = NULL;

Position::list MapManager::GetExpansionLocations()
{
	return this->expansions;
}

Position MapManager::GetClosestExpansionTo(const Position& location)
{
	DEBUG_CHECK(expansions.size() != 0);

	auto result = expansions[0];
	for (auto exp : expansions)
	{
		if (location.getDistance(result) > location.getDistance(exp))
		{
			result = exp;
		}
	}
	return result;
}
Position::list MapManager::GetChokepoints()
{
	return chokepoints;
}

Position MapManager::GetChokepointBetween(Position& start, Position& end)
{
	auto path = getPath(start, end);

	for (auto tile : path)
	{
		auto region = Broodwar->getRegionAt(tile);
		if (region->getDefensePriority() > 0)
		{
			return region->getCenter();
		}
	}
	return Position(Broodwar->self()->getStartLocation());
}

Position MapManager::SuggestBuildingLocation(UnitType type)
{
	auto startLocation = Broodwar->self()->getStartLocation();
	auto res = Broodwar->getBuildLocation(type, startLocation);
	std::cout << res;
	return Position(res);
}

struct node;
typedef std::shared_ptr<node> pNode;

struct node
{
	Region current;
	pNode previous;
	node() : current(NULL), previous(NULL){};
	node(Region c, pNode p) : current(c), previous(p){};
	int getID() { return current->getID(); }
	bool operator==(const node& rhs) { return current->getID() == rhs.current->getID(); }
};

Position::list assemblePath(pNode end)
{
	auto cur = end;
	Position::list result = Position::list();
	while (cur) {
		result.push_front(cur->current->getCenter());
		cur = cur->previous;
	}
	return result;
}

Position::list MapManager::getPath(Position& start, Position& end)
{
	std::queue<pNode> work;
	std::map<int, bool> visited;
	auto s = pNode(new node(Broodwar->getRegionAt(start), NULL));
	auto e = pNode(new node(Broodwar->getRegionAt(end), NULL));

	work.push(s);
	visited.insert(std::make_pair(s->getID(), true));

	while (!work.empty())
	{
		auto cur = work.front();
		work.pop();

		if (*(cur.get()) == *e)
		{
			return assemblePath(cur);
		}

		auto neighbours = cur->current->getNeighbors();
		for (Region c : neighbours)
		{
			auto a = pNode(new node(c, cur));
			if (c->getRegionGroupID() == cur->current->getRegionGroupID() &&
				visited.find(c->getID()) == visited.end()) // if they are walkable neighbour and not visited
			{
				work.push(a);
				visited.insert(std::make_pair(c->getID(), true));
			}
		}
	}
	return Position::list();
	//visited.insert(std::make_pair(startId, true));
	//work.push(start);
}


void MapManager::CalculateChokepoints()
{
	auto start = Broodwar->getRegionAt(Position(Broodwar->self()->getStartLocation()));
	auto initialValues = new std::map<int, int>;
	auto mapWidth = Broodwar->mapWidth();
}