#ifndef SPY_MANAGER_H
#define SPY_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "Agent.h"
#include "MapManager.h"
#include <vector>
#include <queue>
#include <assert.h>

using namespace BWAPI;
class SpyManager;

void moveUnit(Unit u, Position pos)
{
	if (u->isIdle())
		u->move(pos);
}

class AgentSpy : public Agent, public DoSomeThingInRange {
public:
	AgentSpy(Unit u, const Position& target, PtrUnitFilter filter = IsEnemy) :
		Agent(u), DoSomeThingInRange(0, filter), location(target), complete(false){};
	bool OnFrame() override {
		auto res = false;

		// TODO make it more advanced
		if (mUnit->getPosition().getDistance(location) > 100 && !complete) {
			moveUnit(mUnit, location);
		}
		else {
			complete = true;
			moveUnit(mUnit, Position(Broodwar->self()->getStartLocation()));
		}
		return res;
	};
	void OnDraw() override{};
private:
	Position location;
	bool complete;
};

class ScoutPattern : public ControlPattern {
public:
	ScoutPattern(Unit spyUnit, Position location) : ControlPattern(spyUnit) {
		Agents.push_back(new AgentSpy(spyUnit, location));
		Agents.push_back(new AgentStayAway(spyUnit, 50, 100));
	};

	void SetLocation(const Position& pos) { location = pos; }
protected:
	Position location;
};

class SpyManager : public ManagerBase
{
public:
	virtual void OnFrame() override {
		for (auto u : spyUnits)
			u->OnFrame();
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
	static const SpyManager& GetInstance()
	{
		if (!insta)
		{
			insta = new SpyManager;
		}
		return *insta;
	}

	void ExploreLocation(Unit spyUnit, const Position& location) {
		spyUnits.push_back(new ScoutPattern(spyUnit, location));
	}
	void GetEnemyArmyApproxPosition();
	Unitset GetEnemyBuildings() {
		return enemyBuildings;
	}
	void GetEnemyBuilding(UnitType type) { // returns invalid position if building is not there or not discovered
		//TODO
	}
	TilePosition::list GetEnemyBases();
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
