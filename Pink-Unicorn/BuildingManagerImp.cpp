#include "BuildingManager.h"
#include "MapManager.h"

void BuildingManager::OnFrame()
{
	for (auto it : TaskQueue::GetInstance())
	{
		if (BuildingTask * temp = dynamic_cast<BuildingTask*>(it.get()))
		{
			ExecuteBuildTask(*temp);
		}
	}
}


void BuildingManager::ExecuteBuildTask(BuildingTask &task)
{
	TaskQueue &tq = TaskQueue::GetInstance();
	if (task.mExecutor )
	{
		if (task.mbTryToReserved &&  Task::AllSubTasksAreDone(task.mSubTasks) && !task.mIsComplete)
		{
			TilePosition buildTile = Broodwar->getBuildLocation(task.mUnitType, Broodwar->self()->getStartLocation());
			task.mExecutor->build(task.mUnitType, task.mpPosition.get() ? TilePosition(task.mpPosition->x, task.mpPosition->y) : buildTile);
			task.mIsComplete = true;
		}
		else
		{
			ResourcePack rp = ResourcePack::NeedFor(task.mUnitType);
			TaskPtr reserve = TaskPtr(new ReserveResourceTask(rp, task.mPriority));
			tq.push_back(reserve);
			task.mSubTasks.push_back(reserve);
		}
	}
	else if (task.mbExecutorTaskIsCommit)
	{
		tq.push_back(TaskPtr(new GetUnitTask(BWAPI::UnitTypes::Protoss_Probe, &task.mExecutor, task.mPriority)));
		task.mbExecutorTaskIsCommit = true;
	}
}