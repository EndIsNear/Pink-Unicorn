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
	if (Broodwar->getFrameCount() > 100 && Broodwar->getFrameCount() % 25 == 0)
	{
		//calculates current expand for cheking
		size_t idx = (Broodwar->getFrameCount() / 25) % m_expands.size();
		RemoveDeadWorkers(idx);
	}
	if (Broodwar->getFrameCount() % 300 == 0)
	{
		BalanceWorkerBetweenExpands();
	}
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
				m_workersCnt++;
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

	for (auto& worker : bestExp.workers)
	{
		//TODO: Smarter check for free worker
		if (!worker->isCarryingMinerals() && !worker->isCarryingGas())
		{
			result = worker;
			bestExp.workers.erase(worker);
			m_workersCnt--;
			return true;
		}
	}
	return false;
}

int WorkerManager::GetNeededWorkers()
{
	int res = 0;
	if (m_maxWorkers < m_workersCnt)
		return res;
	for (auto& b : m_expands)
	{
		res += m_maxMin * b.minerals.size() + m_maxGas * b.gasStations.size() - b.workers.size();
	}
	return res;
}

int WorkerManager::GetNeededWorkersForBase(Unit base)
{
	if (m_maxWorkers < m_workersCnt)
		return 0;
	for (auto& b : m_expands)
	{
		if (b.base == base)
			return m_maxMin * b.minerals.size() + m_maxGas * b.gasStations.size() - b.workers.size();
	}
	return 0;
}

void WorkerManager::RemoveDeadWorkers(size_t idx)
{
	for (auto it = m_expands[idx].workers.begin(); it != m_expands[idx].workers.end();)
	{
		if (!(*it)->exists())
		{
			m_expands[idx].workers.erase(it++);
			m_workersCnt--;
		}
		else
			it++;
	}
}

void WorkerManager::BalanceWorkerBetweenExpands()
{
	if (m_expands.size() < 2)
		return;
	//minimum per expand
	size_t workersPerExp = m_workersCnt/m_expands.size() + 1;

	for (int i = 0; i < m_expands.size()/2; ++i)
	{
		auto res = std::minmax_element(m_expands.begin(), m_expands.end(),
			[](Expand & left, Expand & right){ return left.workers.size() < right.workers.size(); });
		size_t workersToMove = res.second->workers.size() - workersPerExp;
		moveWorkersFromTo(workersToMove, res.second - m_expands.begin(), res.first - m_expands.begin());
	}
}

void WorkerManager::moveWorkersFromTo(size_t toMove, size_t idxFrom, size_t idxTo)
{
	auto& from = m_expands[idxFrom].workers;
	auto& to = m_expands[idxTo].workers;
	size_t cnt = 0;
	for (auto it = from.begin(); it != from.end() && cnt < toMove;)
	{
		if ((*it)->exists() && !(*it)->isGatheringGas() && !(*it)->isCarryingMinerals())
		{
			to.insert(*it);
			//maybe handle fail of gathering
			(*it)->gather(m_expands[idxTo].base->getClosestUnit(Filter::IsMineralField));
			from.erase(it++);
			cnt++;
		}
		else
			it++;
	}
}