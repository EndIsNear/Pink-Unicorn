#include "BuildingManager.h"
#include "MapManager.h"
#include "AllTasks.h"

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
	if (task.mIsComplete || !Task::AllSubTasksAreDone(task.mSubTasks))
		return;

	TaskQueue &tq = TaskQueue::GetInstance();
	if (!task.mbTryToReserved)
	{
		ResourcePack rp = ResourcePack::NeedFor(task.mUnitType);
		TaskPtr reserve = TaskPtr(new ReserveResourceTask(rp, task.mPriority));
		tq.push_back(reserve);
		task.mSubTasks.push_back(reserve);
		task.mbTryToReserved = true;
	}
	else if (!task.mbExecutorTaskIsCommit)
	{
		tq.push_back(TaskPtr(new GetUnitTask(BWAPI::UnitTypes::Protoss_Probe, &task.mExecutor, task.mPriority)));
		task.mbExecutorTaskIsCommit = true;
	}
	else if (task.mExecutor)
	{
		TilePosition buildTile = MapManager::GetInstance().SuggestBuildingLocation(UnitTypes::Protoss_Pylon);//Broodwar->getBuildLocation(task.mUnitType, Broodwar->self()->getStartLocation());
		task.mExecutor->build(task.mUnitType, task.mpPosition.get() ? TilePosition(task.mpPosition->x, task.mpPosition->y) : buildTile);
		task.mIsComplete = true;
		auto exe = TaskPtr(new TakeWorkerTask(task.mExecutor));
		Broodwar->registerEvent([exe](Game* A) { TaskQueue::GetInstance().push_back(exe); }, nullptr, 25 * 10 + 1, 25 * 10);
	}
	
}