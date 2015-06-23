#include "SpyManager.h"

SpyManager * SpyManager::insta = NULL;

void moveUnit(Unit u, Position pos)
{
	if (u->isIdle() || u->isGatheringMinerals())
		u->move(pos);
}

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

bool SpyManager::IsUnitScouting(Unit u) {
	for (auto sp : spyUnits) {
		if (sp->GetUnit() == u)
			return true;
	}
	return false;
}
void SpyManager::StopUnit(Unit u) {
	for (int i = 0; i < spyUnits.size(); ++i) {
		if (spyUnits[i]->GetUnit() == u) {
			delete *(spyUnits.begin() + i);
			spyUnits.erase(spyUnits.begin() + i);
			return;
		}
	}
}