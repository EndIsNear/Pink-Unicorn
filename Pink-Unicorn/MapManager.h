#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "TasksQueue.h"
#include <vector>
#include <assert.h>

using namespace BWAPI;
struct ExpansionLocation
{
	Position baseLocation;
	Position resourseGroupCenter;
	int radius;

	ExpansionLocation(TilePosition b = TilePositions::None, Position rgc = Positions::None, int r = 200) : 
		baseLocation(b), resourseGroupCenter(rgc), radius(r) {};
};

class MapManager : public ManagerBase
{
public:
	virtual void OnFrame();
	void OnUnitComplete(Unit unit);
	void OnUnitDestroy(Unit unit);
	virtual void CheckForNewTask() override {};
	static Position::list getPath(Position& s, Position& e);
	static MapManager& GetInstance()
	{
		if (!insta)
		{
			insta = new MapManager;
		}
		return *insta;
	}

	Position::list GetExpansionLocations();
	Position GetClosestExpansionTo(const Position& location);
	Position::list GetChokepoints();
	Position GetChokepointBetween(Position& start, Position& end);
	Position GetBaseExit();
	TilePosition SuggestBuildingLocation(UnitType type, TilePosition preferredPosition = start, int radius = 64, bool creep = false);
private:
	MapManager(){
		start = Broodwar->self()->getStartLocation();
		CalculateExpansions();
		ScanPylonBuildGrid();
	};
	MapManager& operator = (const MapManager &m){};
	static MapManager * insta;
	static TilePosition start;

	std::vector<ExpansionLocation> expansions;
	Position::list chokepoints;
	std::set<TilePosition> pylonLocationsGrid; // <TilePosition location, bool occupied>
	std::set<TilePosition> builtPylons;
	TilePosition nextPylon;
	TilePosition nextPylonBuildSpot;

	void ScanPylonBuildGrid();
	void InsertToPylonGrid(TilePosition pos);
	Position GetResourseGroupCenter(TilePosition expansionLocation = start);
	void CalculateExpansions();
	void CalculateChokepoints();
	void getNextPylonBuildSpot();
	void getNextPylon();
	TilePosition SuggestPylon(TilePosition location);
	TilePosition SuggestNexus(TilePosition location);
	TilePosition SuggestCanon(TilePosition location);
	TilePosition SuggestAssimilator(TilePosition location);
	TilePosition SuggestRegular(UnitType type, TilePosition location);
};

#endif //MAP_MANAGER_H
