#include <vector>
#include "BWAPI.h"
#include "ManagerBase.h"
using namespace BWAPI;

#include "WorkerManager.h"

void WorkerManager::OnStart()
{
	for (auto &unit : Broodwar->self()->getUnits())
	{
		if (unit->getType().isResourceDepot())
		{
			AddUnit(unit);
		}
	}

	auto& crnMineral = m_expands[0].minerals.begin();

	for (auto &unit : Broodwar->self()->getUnits())
	{
		if (unit->getType().isWorker())
		{
			if (unit->gather(*crnMineral))
			{
				crnMineral++;
				m_expands[0].workers.insert(unit);
			}
			else
			{
				Broodwar << Broodwar->getLastError() << std::endl;
			}
		}
	}
}

void WorkerManager::OnFrame()
{
	TaskList tasks;
	TaskQueue::GetInstance().GetTasksWithType(Task::TakeControl, tasks);

	for (auto task : tasks)
	{
		auto& tmp = dynamic_cast<TakeWorkerTask*>(task.get())->mUnit;
		if (tmp->isCompleted())
		{
			AddUnit(tmp);
			task->mIsComplete = true;
		}
	}
}

void WorkerManager::AddUnit(Unit unit)
{
	if (unit->getType().isWorker())
	{
		auto nearBase = unit->getClosestUnit(Filter::IsResourceDepot);
		for (auto& expand : m_expands)
		{
			if (expand.base == nearBase)
			{
				expand.workers.insert(unit);
				//TODO: check max workers per mineral/gas
				//for (auto& mineral : expand.minerals)
				//{
				//	if (mineral->isBeingGathered())
				//	{
				//		unit->gather(mineral);
				//		return;
				//	}
				//}
				if (unit->gather(unit->getClosestUnit(Filter::IsMineralField)))
				{
					return;
				}
				else
				{
					Broodwar << Broodwar->getLastError() << std::endl;
				}
			}
		}

		
	}
	else if (unit->getType().isResourceDepot())
	{
		size_t end = m_expands.size();
		m_expands.push_back(Expand());

		m_expands[end].base = unit;
		m_expands[end].minerals = unit->getUnitsInRadius(256, Filter::IsMineralField);
		//m_expands[end].gasStations = unit->getUnitsInRadius(16, Filter::IsRefinery || );
	}
	else if (unit->getType().isRefinery())
	{
		//Filter::IsResourceDepot
	}
}
