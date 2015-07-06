#include "SpyManager.h"

SpyManager * SpyManager::insta = NULL;

void moveUnit(Unit u, Position pos)
{
	if (u->isIdle() || u->isGatheringMinerals())
		u->move(pos);
}

bool AgentExploreBuildings::OnFrame()  {
	auto buildingsInRange = Broodwar->getUnitsInRadius(mUnit->getPosition(),
		400, mFilter);

	Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, 400, Colors::Red);

	for (auto b : buildingsInRange) {
		if (!discovered(b) && b->getType().isBuilding()) {
			toExplore.push_front(b);
			discoveredBuildings.insert(b);
		}
	}

	if (next) {
		moveUnit(mUnit, next->getPosition());
		//mUnit->move(next->getPosition());
		if (mUnit->getDistance(next) < 70) {
			next = NULL;
		}
	}
	else {
		if (toExplore.size()) {
			next = toExplore.back();
			toExplore.pop_back();
			toExplore.push_front(next);
		}
	}
	return false;
};

bool AgentSpy::OnFrame() {

	auto res = false;
	if (nextLocation == TilePositions::None) {
		for (int i = 0; i < locations.size() && !Broodwar->hasPath(mUnit->getPosition(), Position(nextLocation)); ++i) {
			nextLocation = locations.back();
			locations.pop_back();
			if (patrol) {
				locations.push_front(nextLocation);
			}
		}
	}

	//std::cout << Broodwar->self()->getStartLocation() << std::endl;
	if (mUnit->getPosition().getDistance(Position(nextLocation)) > 100 && !complete) {
		moveUnit(mUnit, Position(nextLocation));
		//mUnit->move(Position(nextLocation));
	}
	else {
		if (!locations.size() && !patrol && !complete) {
			moveUnit(mUnit, Position(Broodwar->self()->getStartLocation()));
			complete = true;
		}
		else {
			nextLocation = TilePositions::None;
		}
	}
	return res;
};

Unit GetClosestBuilding(std::map<Unit, TilePosition>& buildings, const TilePosition& pos) {
	for (auto b : buildings) {
		if (b.second.getDistance(pos) < 20)
			return b.first;
	}
	return NULL;
}
TilePosition::list SpyManager::GetEnemyBases()
{
	auto result = TilePosition::list();
	auto rgs = MapManager::GetInstance().GetResourseGroups();
	for (auto e : rgs)
	{
		auto closestUnit = GetClosestBuilding(enemyBuildings, e);
		if (closestUnit) {
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
