#include "MapManager.h"

using namespace BWAPI;

MapManager * MapManager::insta = NULL;

const MapRegionContainer& MapManager::GetExpansionLocations()
{
	return this->expansions;
}

const MapRegion& MapManager::GetClosestExpansionTo(const MapRegion& location)
{
	assert(expansions.size() != 0);

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
const MapRegionContainer& MapManager::GetChokepoints()
{
	return chokepoints;
}
const MapRegion& MapManager::GetClosestChokepointTo(const MapRegion& location)
{
	assert(chokepoints.size() != 0);

	auto result = chokepoints[0];
	for (auto chp : chokepoints)
	{
		if (location.getDistance(result) > location.getDistance(chp))
		{
			result = chp;
		}
	}
	return result;
}

const MapRegion& MapManager::SuggestBuildingLocation(UnitType type)
{
	auto startLocation = Broodwar->self()->getStartLocation();
	auto res = Broodwar->getBuildLocation(type, startLocation);
	return res;
}
