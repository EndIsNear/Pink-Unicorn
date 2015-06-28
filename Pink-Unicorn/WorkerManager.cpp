#include <vector>
#include "BWAPI.h"
#include "ManagerBase.h"

#include "WorkerManager.h"
#include "ProductionManager.h"

WorkerManager * WorkerManager::m_instance = NULL;

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

}

void WorkerManager::AddUnit(Unit unit)
{
	if (unit->getType().isWorker())
	{
		auto nearBase = unit->getClosestUnit(Filter::IsResourceDepot && Filter::IsCompleted);
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
				if (unit->gather(expand.base->getClosestUnit(Filter::IsMineralField)))
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
	}
	else if (unit->getType().isRefinery())
	{
		Unit nearBase = unit->getClosestUnit(Filter::IsResourceDepot);
		for (auto& expand : m_expands)
		{
			if (expand.base == nearBase)
			{
				expand.gasStations.insert(unit);
				size_t cnt = 0;
				for (Unitset::iterator it = expand.workers.begin(); cnt < m_maxGas && it != expand.workers.end(); ++it)
				{
					if (!(*it)->isCarryingMinerals() && !(*it)->isCarryingGas() && !(*it)->isGatheringGas())
					{
						(*it)->gather(unit);
						cnt++;
					}
				}
			}
		}
	}
}

bool WorkerManager::ReleaseWorker(Position pos, Unit& result)
{
	//TODO: use position in getUnitTask (just uncoment)

	//Position wantedPos = tmp->mPosition;
	auto& bestExp = m_expands[0];
	//int minDist = bestExp.base->getDistance(wantedPos);
	//for (auto& expand : m_expands)
	//{
	//	auto tmp = expand.base->getDistance(wantedPos);
	//	if (minDist > tmp)
	//	{
	//		bestExp = expand;
	//		minDist = tmp;
	//	}
	//}

	// basi grozniq kod
	for (auto& worker : bestExp.workers)
	{
		//TODO: Smarter check for free worker
		if (!worker->isCarryingMinerals() && !worker->isCarryingGas())
		{
			result = worker;
			bestExp.workers.erase(worker);
			return true;
		}
	}
	return false;
}

int WorkerManager::GetNeededWorkers()
{
	int res = 0;
	for (auto& b : m_expands)
	{
		res += m_maxMin * b.minerals.size() + m_maxGas * b.gasStations.size() - b.workers.size();
	}
	return res;
}

int WorkerManager::GetNeededWorkersForBase(Unit base)
{
	for (auto& b : m_expands)
	{
		if (b.base == base)
			return m_maxMin * b.minerals.size() + m_maxGas * b.gasStations.size() - b.workers.size();
	}
	return 0;
}


