#include "MapManager.h"
#include <queue>
#include <memory>
#include <Windows.h>
using namespace BWAPI;
using namespace std::placeholders;

MapManager * MapManager::insta = NULL;
TilePosition MapManager::start = TilePositions::None;
Unit MapManager::testWorker = NULL;

void AnalyzeMap();

void MapManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 50 == 0)
	{
		ScanPylonBuildGrid();
		//CalculateExpansions();
	}
	//GetExpansionLocations();

	//GetBaseExit();
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

TilePosition GetBaseLocation(const TilePosition& resGroupCenter);

TilePosition::list MapManager::GetExpansionLocations()
{
	auto result = TilePosition::list();
	for (auto rg : resourseGroups)
	{
		auto base = GetBaseLocation(rg);
		Broodwar->drawCircle(CoordinateType::Map, rg.x * 32, rg.y * 32, 20, Colors::Blue);
		Broodwar->drawCircle(CoordinateType::Map, base.x * 32, base.y * 32, 20, Colors::Green);
		result.push_back(GetBaseLocation(rg));
	}
	return result;
}

TilePosition MapManager::GetNextExpansionLocation()
{
	if (resourseGroups.size())
	{
		auto nextRg = resourseGroups.back();
		resourseGroups.pop_back();
		return GetBaseLocation(nextRg);
	}
	return TilePositions::Invalid;
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

Position getOuterStartLocation()
{
	for (auto s : Broodwar->getStartLocations())
	{
		if (s != Broodwar->self()->getStartLocation())
		{
			return Position(s);
		}
	}
	return Positions::Invalid;
}

Position MapManager::GetBaseExit()
{
	auto res = GetChokepointBetween(Position(Broodwar->self()->getStartLocation()), getOuterStartLocation());
	//Broodwar->drawCircle(CoordinateType::Map, res.x, res.y, 20, Colors::Red);
	return res;
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

std::vector<TilePosition> getNeighbours(TilePosition pos)
{
	auto res = std::vector<TilePosition>();
	res.push_back(TilePosition(pos.x, pos.y-1));
	res.push_back(TilePosition(pos.x - 1, pos.y - 1));
	res.push_back(TilePosition(pos.x + 1, pos.y - 1));
	res.push_back(TilePosition(pos.x - 1, pos.y));
	res.push_back(TilePosition(pos.x + 1, pos.y));
	res.push_back(TilePosition(pos.x, pos.y + 1));
	res.push_back(TilePosition(pos.x - 1, pos.y + 1));
	res.push_back(TilePosition(pos.x + 1, pos.y + 1));
	return res;
}
bool MapManager::hasUnbuildableNeighbors(TilePosition pos)
{
	for (auto p : getNeighbours(pos))
	{
		if (!canBuild(UnitTypes::Protoss_Pylon, p))
		{
			return true;
		}
	}
	return false;
}

TilePosition MapManager::SuggestDefencePylon(const TilePosition& base)
{
	auto width = 5;
	auto exit = GetBaseExit();

	checkTestWorker();
	
	for (auto i = exit.x - width; i < exit.x + width; ++i)
	{
		for (auto j = exit.y - width; j < exit.y + width; ++j)
		{
			auto currentPos = TilePosition(i, j);
			if (canBuild(UnitTypes::Protoss_Pylon, currentPos))
			{
				if (hasUnbuildableNeighbors(currentPos))
					return currentPos;
			}
		}
	}
	return TilePosition(exit.x - 2, exit.y + 2);
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

TilePosition getNextSpot(const TilePosition& location, const std::set<TilePosition>& container)
{
	if (!container.empty())
	{
		auto pred = std::bind([](const TilePosition& target, const TilePosition& l, const TilePosition& r){
			return l.getDistance(target) < r.getDistance(target);
		}, location, _1, _2);
		return *std::min_element(container.begin(), container.end(), pred);
	}
	return TilePositions::Invalid;
}
void MapManager::getNextPylonBuildSpot(const TilePosition& location)
{
	nextPylonBuildSpot = getNextSpot(location, pylonLocationsGrid);
}
void MapManager::getNextPylon(const TilePosition& location)
{
	nextPylon = getNextSpot(location, builtPylons);
}

TilePosition MapManager::SuggestPylon(const TilePosition& location)
{
	getNextPylonBuildSpot(location);
	pylonLocationsGrid.erase(nextPylonBuildSpot); 
	return nextPylonBuildSpot;
}

TilePosition MapManager::SuggestRegular(UnitType type, const TilePosition& location)
{
	if (regularBuildingCount % 2 == 0) // no more than 2 buildings near single pylon
	{
		getNextPylon(location);
	}
	regularBuildingCount++;
	return Broodwar->getBuildLocation(type, nextPylon, 3); // TODO check if that works
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

std::map<int, Unitset>  GetResourseGroups()
{
	auto resourses = Broodwar->getStaticMinerals();
	auto gas = Broodwar->getStaticGeysers();
	for (auto g : gas)
	{
		resourses.insert(g);
	}

	auto resGroups = std::map<int, Unitset>();
	for (auto r : resourses)
	{
		auto rgr = r->getResourceGroup();
		resGroups[rgr].insert(r);
	}
	return resGroups;
}

TilePosition GetResGroupCenter(const Unitset& group)
{
	auto center = Position(0, 0);
	for (auto r : group)
	{
		center += r->getInitialPosition();
	}
	return TilePosition(center/ group.size());
}
std::vector<TilePosition> GetResGroupsCenters(const std::map<int, Unitset> & groups)
{
	auto res = std::vector<TilePosition>();
	for (auto g : groups)
	{
		res.push_back(GetResGroupCenter(g.second));
	}
	return res;
}

TilePosition GetBaseLocation(const TilePosition& resGroupCenter)
{
	std::queue<TilePosition> work;
	std::set<TilePosition> visited;

	work.push(resGroupCenter);
	visited.insert(resGroupCenter);

	while (!work.empty())
	{
		auto cur = work.front();
		work.pop();

		if (Broodwar->canBuildHere(cur, UnitTypes::Protoss_Nexus))
		{
			//Broodwar->drawCircle(CoordinateType::Map, cur.x * 32, cur.y * 32, 20, Colors::Green);
			return cur;
		}

		auto neighbours = getNeighbours(cur);
		for (auto c : neighbours)
		{
			if (visited.find(c) == visited.end())
			{
				work.push(c);
				visited.insert(c);
			}
		}
	}
	return TilePositions::None;
}

void MapManager::CalculateResourseGroups()
{
	auto resGroups = GetResourseGroups();
	resourseGroups = GetResGroupsCenters(resGroups);

	auto pred = std::bind([](const TilePosition& start, const TilePosition& l, const TilePosition& r){
		return start.getDistance(l) > start.getDistance(r);
	}, Broodwar->self()->getStartLocation(), _1, _2);
	std::sort(resourseGroups.begin(), resourseGroups.end(), pred);

	resourseGroups.pop_back(); // that removes the start location's RG, because we dont want to expand there
}

