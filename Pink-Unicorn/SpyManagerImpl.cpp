#include "SpyManager.h"

SpyManager * SpyManager::insta = NULL;

TilePosition::list SpyManager::GetEnemyBases()
{
	auto result = TilePosition::list();
	auto expansions = MapManager::GetInstance().GetExpansionLocations();
	for (auto e : expansions)
	{
		if (Broodwar->getUnitsInRadius(Position(e), 500, (IsEnemy || IsBuilding)).size() != 0)
			result.push_back(e);
	}
	return result;
}