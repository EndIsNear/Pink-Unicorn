#include <vector>
#include "BWAPI.h"
#include "ManagerBase.h"
using namespace BWAPI;

#include "WorkerManager.h"

void WorkerManager::OnStart()
{
	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getType().isResourceDepot())
		{
			AddUnit(u);
		}
	}

	for (auto &u : Broodwar->self()->getUnits())
	{
		if (u->getType().isWorker())
		{
			if (u->isIdle())
			{
				AddUnit(u);
			}
		}
	}
}

void WorkerManager::AddUnit(Unit unit)
{
	if (unit->getType().isWorker())
	{
		auto nearBases = unit->getUnitsInRadius(128, Filter::IsResourceDepot);
		for (auto& expand : m_expands)
		{
			for (auto& nearBase : nearBases)
			{
				if (expand.base == nearBase)
					expand.workers.insert(unit);
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
