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
		auto closestUnit = Broodwar->getClosestUnit(Position(e), (IsEnemy && (IsBuilding || IsBeingConstructed)), 450);
		if (closestUnit) {
			if (enemyBuildings.find(closestUnit) != enemyBuildings.end()) //if unit isn't destroyed
				result.push_back(e);
		}
		
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
void SpyManager::InitialSpy(Unit unit) {
	TilePosition::list outerStartLocations;
	for (auto l : Broodwar->getStartLocations()) {
		if (l != Broodwar->self()->getStartLocation()) {
			outerStartLocations.push_back(l);
		}
	}
	ExploreLocations(unit, outerStartLocations);
}
