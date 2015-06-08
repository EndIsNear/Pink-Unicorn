#include "ProductionManager.h"
#include "ResourceTasks.h"
#include "TasksQueue.h"
#include "DebugStuff.h"
using namespace BWAPI;
using namespace Filter;

void ProduceManager::OnFrame()
{

//	TaskQueue::GetInstance().GetTasksWithType(Task::Produce, myTasks);
	
	TaskList tasks;
	TaskQueue::GetInstance().GetTasksWithType(Task::Produce, tasks);
	//tasks.sort(Task::CompTask);

	for (auto t : tasks)
	{
		if (SingleUnitProduction *temp = dynamic_cast<SingleUnitProduction*>(t.get()))
			ExecuteSingleUnitProduction(*temp);
		else
			DEBUG_CHECK(false);
	}
}


void ProduceManager::ExecuteSingleUnitProduction(SingleUnitProduction &task)
{
	if (task.mAllIsCommit && Task::AllSubTasksAreDone(task.mSubTasks) && !task.mIsComplete)
	{
		auto self = Broodwar->self();
		auto ProductionUnits = self->getUnits().getUnitsInRadius(1000000, (GetType == task.mUnitType.whatBuilds().first) && IsIdle);
		if (!ProductionUnits.empty())
		{
			auto pu = ProductionUnits.begin();
			if(	(*pu)->train(task.mUnitType))
				task.mIsComplete = true;
			// release resource
			ResourcePack rp = ResourcePack::NeedFor(task.mUnitType);
			TaskQueue::GetInstance().push_back(TaskPtr( new ReleaseResourceTask(rp)));
		}
	}
	else if (task.mAllIsCommit == false)
	{
		// for now just reserve resource
		ResourcePack rp = ResourcePack::NeedFor(task.mUnitType);
		auto reserve = TaskPtr(new ReserveResourceTask(rp, task.mPriority));
		TaskQueue::GetInstance().push_back(reserve);
		task.mSubTasks.push_back(reserve);
		task.mAllIsCommit = true;
	}
}


void ProduceManager::FiltratePosibleTasks(TaskList &l) const
{

}


void ProduceManager::ExecuteTask(Task &t) const
{


}