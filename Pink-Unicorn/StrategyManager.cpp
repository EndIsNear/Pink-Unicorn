#include "ManagerBase.h"
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "WorkerManager.h"
#include "BWAPI.h"

#include <algorithm>
#include "StrategyManager.h"

const double WantedFreePopPercents = 10;

StrategyManager * StrategyManager::m_instance = NULL;

void StrategyManager::OnStart()
{
	for (auto &unit : Broodwar->self()->getUnits())
	{
		if (unit->getType().isResourceDepot())
		{
			m_bases.insert(unit);
		}
	}

}

void StrategyManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 10 == 0)
	{
		UpdateSupply();
		UpdateWorkers();
	}
}

void StrategyManager::UpdateSupply()
{
	int freePop = std::max(static_cast<int>(Broodwar->self()->supplyTotal() * (WantedFreePopPercents / 100)), 15);
	if (Broodwar->self()->supplyTotal() + BuildingManager::GetInstance().GetSupplyInProgress() - Broodwar->self()->supplyUsed() < freePop)
	{
		BuildingManager::GetInstance().Build(UnitTypes::Protoss_Pylon);
	}
}

void StrategyManager::UpdateWorkers()
{
	auto& workMan = WorkerManager::GetInstance();
	for (auto& base : m_bases)
	{
		if(workMan.GetNeededWorkersForBase(base) > 0) //crnMaxDiff)
			ProduceManager::GetInstance().ProduceSingleUnitFrom(UnitTypes::Protoss_Probe, base);
	}
}



