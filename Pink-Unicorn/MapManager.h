#ifndef MAP_MANAGER_H
#define MAP_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "TasksQueue.h"
#include <vector>
#include <assert.h>

using namespace BWAPI;
typedef Position::list MapRegionContainer;

class MapManager : public ManagerBase
{
public:
	virtual void OnFrame() override {};
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
	Position SuggestBuildingLocation(UnitType type);
private:
	MapManager(){};
	MapManager& operator = (const MapManager &m){};
	static MapManager * insta;

	MapRegionContainer expansions;
	MapRegionContainer chokepoints;

	void CalculateExpansions()
	{
		auto starts = Broodwar->getStartLocations();
		for (auto p : starts)
		{
			expansions.push_back((Position)p);
		}
	}
	void CalculateChokepoints();
	Position SuggestPylon();
	Position SuggestNexus();
	Position SuggestCanon();
	Position SuggestAssimilator();
	Position SuggestRegular();
};


#endif //MAP_MANAGER_H