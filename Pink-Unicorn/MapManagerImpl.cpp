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

static void drawBox(TilePosition pos, TilePosition dimensions, Unit worker = nullptr) {
	if (pos == TilePositions::None) return;
	auto topRight = Position(pos);
	auto bottomLeft = Position(pos + dimensions);

	if (Broodwar->canBuildHere(pos, UnitTypes::Protoss_Pylon) && Broodwar->isExplored(pos))
	{
		Broodwar->drawBoxMap(topRight, bottomLeft, Colors::Green);
	}
	else {
		Broodwar->drawBoxMap(topRight, bottomLeft, Colors::Red);
	}
}
static void drawGateway(MapManager& mm, TilePosition& p) {
	auto gatewayDim = UnitTypes::Protoss_Gateway.tileSize();
	if (p == TilePositions::None)
		p = mm.SuggestBuildingLocation(UnitTypes::Protoss_Gateway);
	
	drawBox(p, gatewayDim);
}

static TilePosition p1 = TilePositions::None, p2 = TilePositions::None, p3 = TilePositions::None;
void MapManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 100 == 0)
	{
		ScanPylonBuildGrid();
		//CalculateExpansions();
	}

	//drawRegionData();
	//GetDefencePoints(start);
	/*if (Broodwar->getFrameCount() > 2500) {
		drawGateway(*this, p1);
		drawGateway(*this, p2);
		drawGateway(*this, p3);
	}*/
	//GetExpansionLocations();
	//GetChokepoints();
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
		auto tile = unit->getTilePosition();
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
		//Broodwar->drawCircle(CoordinateType::Map, rg.x * 32, rg.y * 32, 20, Colors::Blue);
		//Broodwar->drawCircle(CoordinateType::Map, base.x * 32, base.y * 32, 20, Colors::Green);
		result.push_back(base);
	}
	return result;
}


TilePosition MapManager::GetNextExpansionLocation()
{
	//auto unoccupiedRGs = GetUnoccupiedRGs(resourseGroups);
	if (resourseGroups.size())
	{
		auto nextRg = resourseGroups.back();
		resourseGroups.pop_back();
		resourseGroups.push_front(nextRg); // TODO think how to do it better
		return GetBaseLocation(nextRg);
	}
	return TilePositions::Invalid;
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
	auto res = TilePositions::Invalid;
	auto prefPos = (preferredPosition == TilePositions::None) ? start : preferredPosition;
	if (Broodwar->canBuildHere(preferredPosition, type))
	{
		return preferredPosition;
	}
	else
	{
		if (type == UnitTypes::Protoss_Pylon) {
			return SuggestPylon(prefPos);
		} 
		else if (type == UnitTypes::Protoss_Nexus) {
			return SuggestNexus(prefPos);
		}
		else if (type == UnitTypes::Protoss_Assimilator) {
			return SuggestAssimilator(prefPos);
		}
		else return SuggestRegular(type, prefPos);
	}
	return res;
}

void addPoint(std::vector<TilePosition>& points, const TilePosition& location) {
	if (location.isValid()) {
		points.push_back(location);
	}
}
std::vector<TilePosition> getNeighbours(TilePosition pos)
{
	auto res = std::vector<TilePosition>();
	addPoint(res, TilePosition(pos.x, pos.y-1));
	addPoint(res, TilePosition(pos.x - 1, pos.y - 1));
	addPoint(res, TilePosition(pos.x + 1, pos.y - 1));
	addPoint(res, TilePosition(pos.x - 1, pos.y));
	addPoint(res, TilePosition(pos.x + 1, pos.y));
	addPoint(res, TilePosition(pos.x, pos.y + 1));
	addPoint(res, TilePosition(pos.x - 1, pos.y + 1));
	addPoint(res, TilePosition(pos.x + 1, pos.y + 1));
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


void MapManager::InsertToPylonGrid(const TilePosition& pos)
{
	//Broodwar->drawTextMap((Position)pos, "%d %d", pos.x, pos.y);

	if (testWorker && !testWorker->exists())
	{
		getTestWorker();
	}

	//drawBox(pos, UnitTypes::Protoss_Pylon.tileSize(), testWorker);
	if (Broodwar->canBuildHere(pos, UnitTypes::Protoss_Pylon, testWorker) && Broodwar->isExplored(pos))
	{
		pylonLocationsGrid.insert(pos);
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

TilePosition getNextSpot(const TilePosition& location, std::set<TilePosition>& container)
{
	if (!container.empty())
	{
		auto pred = std::bind([](const TilePosition& target, const TilePosition& l, const TilePosition& r){
			return l.getDistance(target) < r.getDistance(target);
		}, location, _1, _2);
		auto res = *std::min_element(container.begin(), container.end(), pred);
		container.erase(res);
		return res;
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
	return nextPylonBuildSpot;
}

void insertToVect(std::vector<TilePosition>& vect, const TilePosition& p, Color c) {
	//std::cout << p << std::endl;
	Broodwar->drawCircle(CoordinateType::Map, p.x*32, p.y*32, 10, c);
	vect.push_back(p);
}


bool hasSpace(UnitType type, TilePosition pos, int spacing) {
	if (spacing == 0) return true;
	auto buildingCenter = Position(pos) + (Position(type.tileSize()) / 2);
	int buildRadius = buildingCenter.getDistance(Position(pos)) + spacing;
	auto closestUnit = Broodwar->getClosestUnit(buildingCenter, (Filter::IsBuilding || Filter::IsBeingConstructed), buildRadius);
	if (!closestUnit) {
		return true;
	}
	return false;
}

TilePosition getBuildTileInRadius(UnitType type, const TilePosition& center, int radius = 320, int spacing = 30) {
	std::queue<TilePosition> work;
	std::set<TilePosition> visited;
	auto cur = center;
	int iterations = 0;

	if (center.isValid()) {
		work.push(cur);
		visited.insert(center);
		

		while (!work.empty())
		{
			auto cur = work.front();
			work.pop();
			iterations++;
			if (hasSpace(type, cur, spacing) && Broodwar->canBuildHere(cur, type))
			{
				//std::cout << iterations << std::endl;
				return cur;
			}

			auto neighbours = getNeighbours(cur);
			for (auto c : neighbours)
			{
				auto distance = Position(c).getDistance(Position(center));
				if (visited.find(c) == visited.end() && distance < radius)
				{
					work.push(c);
					visited.insert(c);
				}
			}
		}
	}
	std::cout << iterations << std::endl;
	std::cout << " invalid\n";
	return TilePositions::Invalid;
}
TilePosition MapManager::SuggestRegular(UnitType type, const TilePosition& location)
{
	if (nextPylon.isValid()) {
		auto res = getBuildTileInRadius(type, nextPylon, 320);
		if (!res.isValid()) 
		{
			getNextPylon(location);
		}
		return res;
	}
	getNextPylon(location);
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestAssimilator(const TilePosition& location)
{
	auto gayzars = Broodwar->getGeysers();

	if (!gayzars.empty()) { // there are explored and available gayzers and we choose the best one
		auto less = std::bind([](const TilePosition& start, Unit l, Unit r){
			return start.getDistance(l->getTilePosition()) < start.getDistance(r->getTilePosition());
		}, Broodwar->self()->getStartLocation(), _1, _2);
		auto res = *std::min_element(gayzars.begin(), gayzars.end(), less);
		return res->getTilePosition(); 
	}
	return TilePositions::Invalid;
}

TilePosition MapManager::SuggestCanon(const TilePosition& location)
{
	return getBuildTileInRadius(UnitTypes::Protoss_Photon_Cannon, location);
}

TilePosition MapManager::SuggestNexus(const TilePosition& location)
{
	return GetNextExpansionLocation();
}

std::map<int, Unitset> GetResGroups()
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
TilePosition::list GetResGroupsCenters(const std::map<int, Unitset> & groups)
{
	auto res = TilePosition::list();
	for (auto g : groups)
	{
		res.push_back(GetResGroupCenter(g.second));
	}
	return res;
}

TilePosition GetBaseLocation(const TilePosition& resGroupCenter)
{
	return getBuildTileInRadius(UnitTypes::Protoss_Nexus, resGroupCenter, 400, 0);
}

TilePosition::list GetUnoccupiedRGs(const TilePosition::list& rgps) {
	TilePosition::list result = rgps;
	for (int i = 0; i < result.size(); ++i) {
		auto current = result.back();
		result.pop_back();

		if (Broodwar->getUnitsInRadius(Position(current), 400, (Filter::IsResourceDepot || Filter::IsMorphing)).size() != 0) {
			continue;
		}
		result.push_front(current);
	}
	return result;
}

void MapManager::CalculateResourseGroups()
{
	auto resGroups = GetResGroups();
	resourseGroups = GetResGroupsCenters(resGroups);
	auto baseExit = GetBaseExit();

	resourseGroups = GetUnoccupiedRGs(resourseGroups);
	auto pred = [&baseExit](const TilePosition& l, const TilePosition& r){
		return baseExit.getDistance(static_cast<Position>(l)) > baseExit.getDistance(static_cast<Position>(r));
	};
	std::sort(resourseGroups.begin(), resourseGroups.end(), pred);
}

TilePosition::list MapManager::GetDefencePoints(const TilePosition& aroundHere) {
	auto arroundHereNode = getNode(aroundHere);
	TilePosition::list result;
	if (arroundHereNode->regionId == INVALID) {
		// the point is either unwalkable, or a chokepoint
		// if chokepoint - return it
		if (arroundHereNode->value != 0) {
			result.push_back(aroundHere);
		}
	}
	else {
		auto region = regions[arroundHereNode->regionId];
		auto chokepoints = region->getAdjacentChokepoints();

		for (auto c : chokepoints) {
			auto dps = c->getDefencePointsAround();
			for (auto dp : dps) {
				auto node = getNode(dp);

				if (node->regionId != arroundHereNode->regionId) {
					result.push_back(dp);
					Broodwar->drawBoxMap(Position(dp), Position(dp + TilePosition(1, 1)), Colors::Green);
				}
			}
		}
	}

	return result;
}