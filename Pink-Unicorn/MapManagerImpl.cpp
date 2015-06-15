#include "MapManager.h"
#include <queue>
#include <memory>
#include <Windows.h>
using namespace BWAPI;

MapManager * MapManager::insta = NULL;
TilePosition MapManager::start = TilePositions::None;
Unit MapManager::testWorker = NULL;

void MapManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 50 == 0)
	{
		ScanPylonBuildGrid();
		//CalculateExpansions();
	}
	//auto e = SuggestBuildingLocation(UnitTypes::Protoss_Pylon);
	//Sleep(200);
	//std::cout << e << std::endl;
	//Broodwar->drawCircle(CoordinateType::Map, e.x*32, e.y*32, 20, Colors::Red);
	//SuggestBuildingLocation(UnitTypes::Protoss_Nexus, TilePosition(95,42));
}

void MapManager::OnUnitComplete(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Pylon)
	{
		auto tile = unit->getInitialTilePosition();
		builtPylons.insert(tile);
		pylonLocationsGrid.erase(tile);
	}
}

void MapManager::OnUnitDestroy(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Pylon && unit->getPlayer() == Broodwar->self())
	{
		auto tile = unit->getInitialTilePosition();
		builtPylons.erase(tile);
		pylonLocationsGrid.insert(tile);
	}
}
TilePosition::list MapManager::GetExpansionLocations()
{
	auto result = TilePosition::list();
	for (auto exp : expansions)
	{
		result.push_back(exp.baseLocation);
	}
	return result;
}

Position MapManager::GetClosestExpansionTo(const Position& location)
{
	DEBUG_CHECK(expansions.size() != 0);

	auto result = expansions[0].baseLocation;
	for (auto exp : expansions)
	{
		if (location.getDistance(Position(result)) > location.getDistance(Position(exp.baseLocation)))
		{
			result = exp.baseLocation;
		}
	}
	return Position(result);
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

TilePosition MapManager::SuggestBuildingLocation(UnitType type, const TilePosition& preferredPosition, int radius, bool creep)
{
	auto res = Broodwar->getBuildLocation(type, preferredPosition, radius, creep);

	if (Broodwar->canBuildHere(preferredPosition, type))
	{
		return preferredPosition;
	}
	else
	{
		if (type == UnitTypes::Protoss_Pylon) {
			return SuggestPylon(preferredPosition);
		} 
		else if(type == UnitTypes::Protoss_Assimilator) {
			return SuggestAssimilator(preferredPosition);
		} 
		else if (type == UnitTypes::Protoss_Photon_Cannon) {
			return SuggestCanon(preferredPosition);
		} 
		else if (type == UnitTypes::Protoss_Nexus) {
			return SuggestNexus(preferredPosition);
		}
		else return SuggestRegular(type, preferredPosition);
	}
	return res;
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

Position::list MapManager::getPath(const Position& start, const Position& end)
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
}


void MapManager::CalculateChokepoints()
{
	auto start = Broodwar->getRegionAt(Position(Broodwar->self()->getStartLocation()));
	auto initialValues = new std::map<int, int>;
	auto mapWidth = Broodwar->mapWidth();
}

void MapManager::InsertToPylonGrid(const TilePosition& pos)
{
	Broodwar->drawTextMap((Position)pos, "%d %d", pos.x, pos.y);

	if (testWorker && !testWorker->exists())
	{
		getTestWorker();
	}

	if (Broodwar->canBuildHere(pos, UnitTypes::Protoss_Pylon, testWorker) && Broodwar->isExplored(pos))
	{
		pylonLocationsGrid.insert(pos);
		//Broodwar->drawCircle(CoordinateType::Map, pos.x * 32, pos.y * 32, 10, Colors::Green);
	}
	else {
		//Broodwar->drawCircle(CoordinateType::Map, pos.x * 32, pos.y * 32, 10, Colors::Red);
	}
	
}

void MapManager::ScanPylonBuildGrid()
{
	auto mapWidth = Broodwar->mapWidth();
	auto mapHeight = Broodwar->mapHeight();

	for (int i = 0; i < mapWidth; i += 9)
	{
		for (int j = 0; j < mapHeight; j += 8)
		{
			InsertToPylonGrid(TilePosition(i, j));
			InsertToPylonGrid(TilePosition(i + 2, j));
		}
	}
}

Position MapManager::GetResourseGroupCenter(const TilePosition& expansionLocation)
{
	auto resourses = Broodwar->getUnitsInRadius(Position(expansionLocation), 250, Filter::IsResourceContainer);
	auto center = Position(expansionLocation);
	for (auto res : resourses)
	{
		center += res->getInitialPosition();
	}
	center /= resourses.size() + 1;

	Broodwar->drawCircle(CoordinateType::Map, center.x, center.y, 200, Colors::Red);
	return center;
}

void MapManager::CalculateExpansions()
{
	// TODO: preprocess map, save hot point positions and metadata to file and parse the file here
	// meaning this method will change later
	auto starts = Broodwar->getStartLocations();
	for (auto p : starts)
	{
		if (p == Broodwar->self()->getStartLocation())
			expansions.push_back(ExpansionLocation(p, GetResourseGroupCenter(p)));
	}

	expansions.push_back(ExpansionLocation(TilePosition(35, 54)));
	expansions.push_back(ExpansionLocation(TilePosition(65, 78)));
	expansions.push_back(ExpansionLocation(TilePosition(95, 42)));
	expansions.push_back(ExpansionLocation(TilePosition(59, 18)));
}

using namespace std::placeholders;

void MapManager::getNextPylonBuildSpot(const TilePosition& location)
{
	if (!pylonLocationsGrid.empty())
	{
		auto pred = std::bind([](const TilePosition& target, const TilePosition& l, const TilePosition& r){
			return l.getDistance(target) < r.getDistance(target);
		}, location, _1, _2);
		nextPylonBuildSpot = *std::min_element(pylonLocationsGrid.begin(), pylonLocationsGrid.end(), pred);
	}
	else
	{
		nextPylonBuildSpot = TilePositions::Invalid;
	}
}
void MapManager::getNextPylon(const TilePosition& location)
{
	if (!builtPylons.empty())
	{
		auto pred = std::bind([](const TilePosition& target, const TilePosition& l, const TilePosition& r){
			return l.getDistance(target) < r.getDistance(target);
		}, location, _1, _2);
		nextPylon = *std::min_element(builtPylons.begin(), builtPylons.end(), pred);
	}
	else
	{
		nextPylon = TilePositions::Invalid;
	}
}

TilePosition MapManager::SuggestPylon(const TilePosition& location)
{
	getNextPylonBuildSpot(location);
	pylonLocationsGrid.erase(nextPylonBuildSpot); 
	return nextPylonBuildSpot;
}

TilePosition MapManager::SuggestRegular(UnitType type, const TilePosition& location)
{
	getNextPylon(location);
	return Broodwar->getBuildLocation(type, location); // TODO check if that works
}

TilePosition MapManager::SuggestAssimilator(const TilePosition& location)
{
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestCanon(const TilePosition& location)
{
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestNexus(const TilePosition& location)
{
	return TilePositions::Invalid;
}
