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
	AgentExploreBuildings(Unit u, const UnitFilter &filter) :
	Agent(u), DoSomeThingInRange(0, filter), next(NULL){};
	bool OnFrame() override;
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
	AgentSpy(Unit u, const TilePosition::list& targets, bool patrol, const UnitFilter &filter) :
		Agent(u), DoSomeThingInRange(0, filter), locations(targets), complete(false), nextLocation(Positions::None), patrol(patrol){};
	bool OnFrame() override;
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
		Agents.push_back(new AgentStayAway(spyUnit, 200, 200, IsEnemy && !IsBuilding ));
		Agents.push_back(new AgentExploreBuildings(spyUnit, IsEnemy && IsBuilding));
		Agents.push_back(new AgentSpy(spyUnit, locations, patrol, IsEnemy));
	};
};

class SpyManager : public ManagerBase
{
public:
	virtual void OnFrame() override {
		for (auto u : spyUnits)
			u->OnFrame();
		if (Broodwar->getFrameCount() % 100 == 0) {
			//std::cout << GetEnemyBases().size() << std::endl;
		}
		//GetEnemyBases();
		/*Unit w;
		if (WorkerManager::GetInstance().GetInstance().ReleaseWorker(Positions::None, w) && !exploring) {
			ExploreLocations(w, Broodwar->getStartLocations());
			exploring = true;
		}*/
	}

	virtual void OnStart() override {}

	virtual void ReleaseInst() override {
		delete insta;
		insta = NULL;
	}
	void OnUnitShow(Unit u) override {
		if (u->getPlayer() == Broodwar->enemy()) {
			auto position = u->getTilePosition();
			if (u->getType().isBuilding()) {
				enemyBuildings[u] = position;
				//std::cout << "buildng insert " << u->getType() << std::endl;
				//std::cout << u->getTilePosition() << std::endl;
			}
			else
				enemyUnits[u] = position;
		}
	}
	void OnUnitDestroy(Unit u) override {
		if (u->getPlayer() == Broodwar->enemy()) {
			if (u->getType().isBuilding())
				enemyBuildings.erase(u);
			else
				enemyUnits.erase(u);
		}
		std::cout << "unit destroy" << std::endl;
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
	std::map<Unit, TilePosition> GetEnemyBuildings() {
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
	std::map<Unit, TilePosition> enemyBuildings;
	std::map<Unit, TilePosition> enemyUnits;
};

#endif //SPY_MANAGER_H
