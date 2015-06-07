#include "ProductionManager.h"
#include "ResourceTasks.h"
#include "TasksQueue.h"
#include "assert.h"
using namespace BWAPI;
using namespace Filter;

void ProduceManager::OnFrame()
{

//	TaskQueue::GetInstance().GetTasksWithType(Task::Produce, myTasks);
	
	TaskList tasks;
	TaskQueue::GetInstance().GetTasksWithType(Task::Produce, tasks);
	tasks.sort(Task::CompTask);

	for (auto t : tasks)
	{
		if (SingleUnitProduction *temp = dynamic_cast<SingleUnitProduction*>(t.get()))
			ExecuteSingleUnitProduction(*temp);
		else
			assert(false);
	}
}


void ProduceManager::ExecuteSingleUnitProduction(SingleUnitProduction &task)
{
	if (task.SubTasks.empty())
	{
		auto self = Broodwar->self();
		auto ProductionUnits = self->getUnits().getUnitsInRadius(1000000, (GetType == task.mType.whatBuilds().first) && IsIdle);
		if (!ProductionUnits.empty())
		{
			auto pu = ProductionUnits.begin();
			(*pu)->train(task.mType);
			// release resource
			ResourcePack rp = ResourcePack::NeedFor(task.mType);
			TaskQueue::GetInstance().push_back(TaskPtr( new ReleaseResourceTask(rp)));
		}
	}
	else
	{
		// for now just reserve resource
		ResourcePack rp = ResourcePack::NeedFor(task.mType);
		TaskQueue::GetInstance().push_back(TaskPtr(new ReserveResourceTask(rp, task.mPriority)));
	}
}


void ProduceManager::FiltratePosibleTasks(TaskList &l) const
{

}


void ProduceManager::ExecuteTask(Task &t) const
{


}