#include "SpyManager.h"

SpyManager * SpyManager::insta = NULL;
bool exploring = false;

void SpyManager::OnFrame()
{
	if (!exploring)
	{ 
		auto startLocations = Broodwar->getStartLocations();
		auto units = Broodwar->self()->getUnits();
		Unit worker;
		for (auto u : units)
		{
			if (!u->getType().isBuilding())
			{
				worker = u;
			}
		}

		TilePosition exploreLocation;
		for (auto l : startLocations)
		{
			if (!Broodwar->isExplored(l)) exploreLocation = l;
		}

		ExploreLocation((Position)exploreLocation);
		
	}
	CheckTasks(Task::Type::Explore);
}

void SpyManager::CheckTasks(Task::Type type)
{
	auto tq = TaskQueue::GetInstance();
	TaskList tasks;
	tq.GetTasksWithType(type, tasks);
	if (!tasks.empty())
	{
		auto task = (ExploreTask*)((*tasks.begin()).get());
		if (task->Complete())
		{
			task->mIsComplete = true;
		}
	}

	tq.ReleaseCompleteTasks();
}

void SpyManager::OnDiscoverResourse(Unit resPatch)
{

}
Unit SpyManager::RequestUnit(UnitType type)
{
	auto units = Broodwar->self()->getUnits();

	for (auto u : units)
	{
		if (u->getType() == type)
		{
			// TODO check if it's usable (meaning not reserved by other managers)
			ReserveUnit(u);
			return u;
		}
	}
	return nullptr;
}

void SpyManager::ReserveUnit(Unit u)
{
	spyUnits.insert(u);
}

bool SpyManager::IsReserved(Unit unit)
{
	for (auto u : spyUnits)
	{
		if (u->getID() == unit->getID()) return true;
	}
	return false;
}

int SpyManager::ExploreLocation(const Position& p)
{
	auto exploreUnit = RequestUnit(UnitTypes::Protoss_Probe);
	if (!exploreUnit)
	{
		std::cout << "Could not reserve unit" << std::endl;
		return -1;
	}
	auto expTask = new ExploreTask(p, exploreUnit, Task::Priority::MAX);
	expTask->Execute();
	TaskQueue::GetInstance().push_back(TaskPtr(expTask));
	return 0;
}

