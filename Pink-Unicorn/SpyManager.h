#ifndef SPY_MANAGER_H
#define SPY_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "Agent.h"
#include "MapManager.h"
#include "WorkerManager.h"
#include <vector>
#include <queue>
#include <assert.h>

using namespace BWAPI;
class SpyManager;

void moveUnit(Unit u, Position pos);

class AgentExploreBuildings : public Agent, public DoSomeThingInRange {
public:
	AgentExploreBuildings(Unit u, PtrUnitFilter filter = IsEnemy) :
	Agent(u), DoSomeThingInRange(0, filter), next(NULL){};
	bool OnFrame() override {
		auto buildingsInRange = Broodwar->getUnitsInRadius(mUnit->getPosition(),
		400, IsEnemy);

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
			if (mUnit->getDistance(next) < 100) {
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
	void OnDraw() override{};
private:
	bool discovered(Unit b) {
		return discoveredBuildings.find(b) != discoveredBuildings.end();
	}
	Unit next;
	std::deque<Unit> toExplore; // the probe is to circle around buildings in enemy base, providing constant monitoring of everything
	Unitset discoveredBuildings;
};

class AgentSpy : public Agent, public DoSomeThingInRange {
public:
	AgentSpy(Unit u, const TilePosition::list& targets, bool patrol = true, PtrUnitFilter filter = IsEnemy) :
		Agent(u), DoSomeThingInRange(0, filter), locations(targets), complete(false), nextLocation(Positions::None), patrol(patrol){};
	bool OnFrame() override {
		auto res = false;
		if (nextLocation == TilePositions::None) {
			if (locations.size()) {
				nextLocation = locations.back();
				locations.pop_back();
			}
			if (patrol) {
				locations.push_front(nextLocation);
			}
		}
		
		//std::cout << Broodwar->self()->getStartLocation() << std::endl;
		if (mUnit->getPosition().getDistance(Position(nextLocation)) > 170 && !complete) {
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
	void OnDraw() override{};
private:
	TilePosition::list locations;
	TilePosition nextLocation;
	bool complete;
	bool patrol;
};

class ScoutPattern : public ControlPattern {
public:
	ScoutPattern(Unit spyUnit, const TilePosition::list& locations, bool patrol) : ControlPattern(spyUnit) {
		Agents.push_back(new AgentStayAway(spyUnit, 120, 100));
		Agents.push_back(new AgentExploreBuildings(spyUnit));
		Agents.push_back(new AgentSpy(spyUnit, locations, patrol));		
	};
};

class SpyManager : public ManagerBase
{
public:
	//bool exploring = false;
	virtual void OnFrame() override {
		for (auto u : spyUnits)
			u->OnFrame();

		/*Unit w;
		if (WorkerManager::GetInstance().GetInstance().ReleaseWorker(Positions::None, w) && !exploring) {
			ExploreLocations(w, Broodwar->getStartLocations());
			exploring = true;
		}*/
	}
	virtual void ReleaseInst() override {
		delete insta;
		insta = NULL;
	}
	void OnUnitDiscover(Unit u) override {
		if (u->getPlayer() == Broodwar->enemy()) {
			if (u->getType().isBuilding())
				enemyBuildings.insert(u);
			else
				enemyUnits.insert(u);
		}
	}
	static SpyManager& GetInstance()
	{
		if (!insta)
		{
			insta = new SpyManager;
		}
		return *insta;
	}

	void InitialSpy(Unit unit);
	void ExploreLocations(Unit spyUnit, const TilePosition::list& locations, bool patrol = true) {
		spyUnits.push_back(new ScoutPattern(spyUnit, locations, patrol));
	}
	void GetEnemyArmyApproxPosition();
	Unitset GetEnemyBuildings() {
		return enemyBuildings;
	}
	void GetEnemyBuilding(UnitType type) { // returns invalid position if building is not there or not discovered
		//TODO
	}
	TilePosition::list GetEnemyBases();
	bool IsUnitScouting(Unit u);
	void StopUnit(Unit u);
private:
	static SpyManager* insta;
	~SpyManager(){
		for (auto su : spyUnits) {
			delete su;
		}
	}
	std::vector<ControlPattern*> spyUnits;
	Unitset enemyBuildings;
	Unitset enemyUnits;
};

#endif //SPY_MANAGER_H
