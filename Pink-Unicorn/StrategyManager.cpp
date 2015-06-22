#include "ManagerBase.h"
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "WorkerManager.h"
#include "ResourceManager.h"
#include "BWAPI.h"

#include <algorithm>
#include "StrategyManager.h"

const double WantedFreePopPercents = 10;

StrategyManager * StrategyManager::m_instance = NULL;
int gatewayCnt = 0;
int gasCnt = 0;

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
		if (ResourceManager::GetInstance().GetFreeMineral() > 150 && gatewayCnt < 3)
		{
			if (BuildingManager::GetInstance().Build(UnitTypes::Protoss_Gateway))
				gatewayCnt++;
		}
		if (gatewayCnt >= 3 && ResourceManager::GetInstance().GetFreeMineral() > 300)
		{
			for (int i = 0; i < 3; ++i)
				ProduceManager::GetInstance().ProduceSingleUnit(UnitTypes::Protoss_Zealot);
		}
	}

}

void StrategyManager::UpdateSupply()
{
	int wantedSupply = std::max(static_cast<int>(Broodwar->self()->supplyTotal() * (WantedFreePopPercents / 100)), 15);
	if (ResourceManager::GetInstance().GetFreeSupply() + BuildingManager::GetInstance().GetSupplyInProgress() < wantedSupply)
	{
		BuildingManager::GetInstance().BuildBaseExit(UnitTypes::Protoss_Pylon);
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



