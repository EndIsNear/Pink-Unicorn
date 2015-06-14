#include "MacroManager.h"
#include "GameAnalyzer.h"

using namespace BWAPI;

void MacroManager::OnFrame()
{
	GameAnalyzer &ga = GameAnalyzer::GetInstance();
	TaskQueue &tq = TaskQueue::GetInstance();
	double dResValue = ga.GetResourceProductionValue();
	if (dResValue < 0.5)
	{
		tq.push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Probe, Task::MAX)));
	} else if ((dResValue < 0.8))
	{
		tq.push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Probe, Task::HIGH)));
	} else if ((dResValue < 0.5))
	{
		tq.push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Probe, Task::MID)));
	}
	//if (ga.GetMapControlValue() < 1.)
		//TaskQueue::GetInstance().push_back(TaskPtr(new SpyTask)
	if (ga.GetArmyValue() < 10000.)
	{
		tq.push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Zealot, Task::MID)));
	}

	if (ga.GetSupplyValue()  < 1.)
	{
		tq.push_back(TaskPtr(new BuildingTask(BWAPI::UnitTypes::Protoss_Pylon, Task::MAX)));
	}

	TaskList l;
	tq.GetTasksWithType(Task::Requirement, l);

	for (auto it : l)
	{
		if (RequireBuilding *temp = dynamic_cast<RequireBuilding*>(it.get()))
		{
			ExecuteRequireBuildinTask(*temp);
		}
	}

}


void MacroManager::ExecuteRequireBuildinTask(RequireBuilding &task)
{
	TaskQueue::GetInstance().push_back(TaskPtr(new BuildingTask(task.mBuildingType, task.mPriority)));
	task.mIsComplete = true;
}
