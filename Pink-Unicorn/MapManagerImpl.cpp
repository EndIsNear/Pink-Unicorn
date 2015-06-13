#include "MapManager.h"
#include <queue>
#include <Windows.h>
using namespace BWAPI;

MapManager * MapManager::insta = NULL;
TilePosition MapManager::start = TilePositions::None;

void MapManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 800 == 0)
	{
		ScanPylonBuildGrid();
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
Position::list MapManager::GetExpansionLocations()
{
	auto result = Position::list();
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
		if (location.getDistance(result) > location.getDistance(exp.baseLocation))
		{
			result = exp.baseLocation;
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

TilePosition MapManager::SuggestBuildingLocation(UnitType type, TilePosition preferredPosition, int radius, bool creep)
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
}


void MapManager::CalculateChokepoints()
{
	auto start = Broodwar->getRegionAt(Position(Broodwar->self()->getStartLocation()));
	auto initialValues = new std::map<int, int>;
	auto mapWidth = Broodwar->mapWidth();
}

void MapManager::InsertToPylonGrid(TilePosition pos)
{
	Broodwar->drawTextMap((Position)pos, "%d %d", pos.x, pos.y);
	auto workers = Broodwar->getUnitsInRadius(Position(start), 2000, Filter::IsWorker);
	auto w = *workers.begin();

	if (Broodwar->canBuildHere(pos, UnitTypes::Protoss_Pylon, w) && Broodwar->isExplored(pos))
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

	for (int i = 0; i < mapWidth; i += 7)
	{
		for (int j = 0; j < mapHeight; j += 10)
		{
			InsertToPylonGrid(TilePosition(i, j));
			InsertToPylonGrid(TilePosition(i + 2, j));
		}
	}
}

Position MapManager::GetResourseGroupCenter(TilePosition expansionLocation)
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
}

void MapManager::getNextPylonBuildSpot()
{
	nextPylonBuildSpot = *std::min_element(pylonLocationsGrid.begin(), pylonLocationsGrid.end(), [](TilePosition l, TilePosition r){
		return l.getDistance(start) < r.getDistance(start);
	});
}
void MapManager::getNextPylon()
{
	nextPylon = *std::min_element(builtPylons.begin(), builtPylons.end(), [](TilePosition l, TilePosition r){
		return l.getDistance(start) < r.getDistance(start);
	});
}

TilePosition MapManager::SuggestPylon(TilePosition location)
{
	getNextPylonBuildSpot();
	pylonLocationsGrid.erase(nextPylonBuildSpot); 
	return nextPylonBuildSpot;
}

TilePosition MapManager::SuggestRegular(UnitType type, TilePosition location)
{
	getNextPylon();
	return Broodwar->getBuildLocation(type, location); // TODO check if that works
}

TilePosition MapManager::SuggestAssimilator(TilePosition location)
{
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestCanon(TilePosition location)
{
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestNexus(TilePosition location)
{
	return TilePositions::Invalid;
}
