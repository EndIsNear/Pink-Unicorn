#ifndef SPY_MANAGER_H
#define SPY_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include <vector>
#include <queue>
#include <assert.h>

using namespace BWAPI;
class SpyManager;
class ExploreTask;

struct ResGroup;
typedef std::vector<ResGroup> ResGroupSet;

struct Enemy; // TODO

class ArmyGroup; // TODO
typedef std::vector<ArmyGroup> ArmyGroupSet;

/********************************/
class SpyManager : public ManagerBase
{
public:
	virtual void OnFrame();
	virtual void CheckForNewTask();
	void CheckTasks(Task::Type type);
	void OnDiscoverResourse(Unit resPatch);
	void OnDiscoverEnemy(Unit enemy);

	static const SpyManager& GetInstance()
	{
		if (!insta)
		{
			insta = new SpyManager;
		}
		return *insta;
	}

	int ExploreLocation(const Position& location);
	bool IsReserved(Unit unit);

	void GetResGroups();
	void GetEnemyArmyApproxPosition();
	void GetEnemyBuildings(); // returns discovered buildings
	void GetEnemyBuilding(UnitType type); // returns invalid position if building is not there or not discovered
	void GetExpansionLocations(); 
private:
	Unit RequestUnit(UnitType type);
	void ReserveUnit(Unit unit);
	void ReleaseUnit(int unitID);
private:
	static SpyManager* insta;
private:
	Unitset spyUnits;
	Unitset enemyBuildings;
	Unitset enemyUnits;
	ResGroupSet resourses;
};

class ExploreTask : public Task
{
public:
	ExploreTask(const Position& l, Unit controlUnit, Task::Priority pri)
	{
		mPriority = pri;
		Task::mType = Task::Explore;

		this->location = l;
		this->unit = controlUnit;
	}

	bool CanAccept()
	{
		return unit->isFlying() ? true : (Broodwar->getRegionAt(location)->getRegionGroupID() == 
			Broodwar->getRegionAt(unit->getPosition())->getRegionGroupID());
	}

	bool Complete()
	{
		return unit->getPosition() == location;
	}

	bool Cancel()
	{
		return false;
	}

	void Execute()
	{
		unit->move(location);
	}
public:
	Unit unit;
	Position location;
};

struct ResGroup
{
public:
	ResGroup(){
		resGrpId = -1;
	}

	Position GetCenter(); 
	Position GetExpansionLocation()
	{

	};
	bool ContainsResourse(Unit res)
	{
		Broodwar->getUnitsInRadius(res->getPosition(), ResGroupRadius, Filter::ResourceGroup);
		return resourses.find(res) != resourses.end();
	}
	bool CanInsert(Unit res)
	{
		return resGrpId == -1 || (res->getResourceGroup() == resGrpId && !ContainsResourse(res));
	}
	void InsertResourse(Unit res)
	{
		if ( CanInsert(res))
		{
			resourses.insert(res);
		}
	}
public:
	int resGrpId;
	Unitset resourses;
	static const int ResGroupRadius = 1000;
};

#endif //SPY_MANAGER_H
