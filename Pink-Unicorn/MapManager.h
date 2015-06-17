#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include <vector>
#include <assert.h>

using namespace BWAPI;

class MapManager : public ManagerBase
{
public:
	virtual void OnFrame();
	virtual void OnUnitComplete(Unit unit);
	virtual void OnUnitDestroy(Unit unit);
	virtual void ReleaseInst() override{
		delete insta;
		insta = NULL;
	};
	static Position::list getPath(const Position& s, const Position& e);
	static MapManager& GetInstance()
	{
		if (!insta)
		{
			insta = new MapManager;
		}
		return *insta;
	}

	TilePosition::list GetExpansionLocations();
	TilePosition GetNextExpansionLocation();
	Position::list GetChokepoints();
	Position GetChokepointBetween(Position& start, Position& end);
	Position GetBaseExit();
	TilePosition SuggestBuildingLocation(UnitType type, const TilePosition& preferredPosition = start, int radius = 1000, bool creep = false);
	TilePosition SuggestDefencePylon(const TilePosition& base = start); // the function determines the most critical chokepoint and gets pylon location next to it
	TilePosition SuggestDefenceCannon(const TilePosition& base = start);
private:
	MapManager(){
		regularBuildingCount = 0;
		start = Broodwar->self()->getStartLocation();
		CalculateResourseGroups();
		ScanPylonBuildGrid();
	};
	MapManager& operator = (const MapManager &m){};
	static MapManager * insta;
	static TilePosition start;
	static Unit testWorker; // used to better check if given position is buildable or nots
	void checkTestWorker()
	{
		if (!testWorker || !testWorker->exists())
		{
			getTestWorker();
		}
	}

	void getTestWorker()
	{
		auto workers = Broodwar->getUnitsInRadius(Position(start), 10000, Filter::IsWorker);
		for (auto w : workers)
		{
			if (w->exists())
			{
				testWorker = w;
				return;
			}
		}
	}

	bool canBuild(UnitType type, const TilePosition& location)
	{
		checkTestWorker();
		return Broodwar->canBuildHere(location, type, testWorker);
	}
	bool hasUnbuildableNeighbors(TilePosition pos);
	
	TilePosition::list resourseGroups;
	Position::list chokepoints;
	std::set<TilePosition> pylonLocationsGrid; // <TilePosition location, bool occupied>
	std::set<TilePosition> builtPylons;
	TilePosition nextPylon;
	TilePosition nextPylonBuildSpot;
	int regularBuildingCount;

	void ScanPylonBuildGrid();
	void InsertToPylonGrid(const TilePosition& pos);
	Position GetResourseGroupCenter(const TilePosition& expansionLocation = start);
	void CalculateResourseGroups();
	void CalculateChokepoints();
	void getNextPylonBuildSpot(const TilePosition& location);
	void getNextPylon(const TilePosition& location);
	TilePosition SuggestPylon(const TilePosition& location);
	TilePosition SuggestNexus(const TilePosition& location);
	TilePosition SuggestCanon(const TilePosition& location);
	TilePosition SuggestAssimilator(const TilePosition& location);
	TilePosition SuggestRegular(UnitType type, const TilePosition& location);
};

#endif //MAP_MANAGER_H
