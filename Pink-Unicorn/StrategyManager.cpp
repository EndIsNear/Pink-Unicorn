#include "ManagerBase.h"
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "WorkerManager.h"
#include "ResourceManager.h"
#include "MapManager.h"
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
	PushBuildTask(11, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	PushProductionTask(10, UnitTypes::Protoss_Zealot);
	PushBuildTask(9, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	PushBuildTask(9, UnitTypes::Protoss_Assimilator, Broodwar->self()->getStartLocation());
	PushProductionTask(8, UnitTypes::Protoss_Zealot);
	PushProductionTask(8, UnitTypes::Protoss_Zealot);
	PushBuildTask(7, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	PushBuildTask(7, UnitTypes::Protoss_Cybernetics_Core, Broodwar->self()->getStartLocation());
	for (int i = 0; i < 100; ++i)
	{
		PushProductionTask(6, UnitTypes::Protoss_Zealot);
		PushProductionTask(6, UnitTypes::Protoss_Dragoon);
	}

}

void StrategyManager::OnFrame()
{
	if (Broodwar->getFrameCount() % 10 == 0)
	{
		UpdateSupply();
		UpdateWorkers();
		CheckQueue();
	}
}

void StrategyManager::UpdateSupply()
{
	size_t wantedSupply = std::max(static_cast<int>(Broodwar->self()->supplyTotal() * (WantedFreePopPercents / 100)), 15);
	if (ResourceManager::GetInstance().GetFreeSupply() + BuildingManager::GetInstance().GetSupplyInProgress() < wantedSupply+10)
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

bool StrategyManager::BuildExpand()
{
	if (ResourceManager::GetInstance().GetFreeMineral() < 400)
		return false;
	auto pos = MapManager::GetInstance().GetNextExpansionLocation();
	return BuildingManager::GetInstance().BuildNearTo(UnitTypes::Protoss_Nexus, pos);
}

void StrategyManager::PushBuildTask(int priority, UnitType type, TilePosition nearTo)
{
	m_TaskQ.emplace(priority,
		[type, nearTo]()
	{
		return BuildingManager::GetInstance().BuildNearTo(type, nearTo);
	}
	);
}

void StrategyManager::PushProductionTask(int priority, UnitType type/*, TilePosition nearTo*/)
{
	m_TaskQ.emplace(priority,
		[type]()
	{
		return ProduceManager::GetInstance().ProduceSingleUnit(type);
	}
		);
}

void StrategyManager::PushExpandTask(int priority)
{
	m_TaskQ.emplace(priority,
		[]()
	{
		return StrategyManager::GetInstance().BuildExpand();
	}
	);
}

void StrategyManager::CheckQueue()
{
	if (!m_TaskQ.empty() && m_TaskQ.top().second())
		m_TaskQ.pop();
}

