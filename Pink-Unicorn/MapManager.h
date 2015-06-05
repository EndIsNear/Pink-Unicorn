#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "TasksQueue.h"
#include <vector>
#include <assert.h>

using namespace BWAPI;
typedef TilePosition MapRegion;
typedef TilePosition::list MapRegionContainer;

class MapManager : public ManagerBase
{
public:
	virtual void OnFrame() override {};
	virtual void CheckForNewTask() override {};

	static MapManager& GetInstance()
	{
		if (!insta)
		{
			insta = new MapManager;
		}
		return *insta;
	}

	const MapRegionContainer& GetExpansionLocations();
	const MapRegion& GetClosestExpansionTo(const MapRegion& location);
	const MapRegionContainer& GetChokepoints();
	const MapRegion& GetClosestChokepointTo(const MapRegion& location);
	const MapRegion& GetBaseExit();
	const MapRegion& SuggestBuildingLocation(UnitType type);
private:
	MapManager(){};
	MapManager& operator = (const MapManager &m){};
	static MapManager * insta;

	MapRegionContainer expansions;
	MapRegionContainer chokepoints;

	void CalculateExpansions()
	{
		expansions = Broodwar->getStartLocations();
	}
	void CalculateChokepoints();
	const MapRegion& SuggestPylon();
	const MapRegion& SuggestNexus();
	const MapRegion& SuggestCanon();
	const MapRegion& SuggestAssimilator();
	const MapRegion& SuggestRegular();
};


#endif //MAP_MANAGER_H