#include "ManagerBase.h"
#include "BuildingManager.h"
#include "ProductionManager.h"
#include "WorkerManager.h"
#include "ResourceManager.h"
#include "MapManager.h"
#include "BWAPI.h"

#include <algorithm>
#include "StrategyManager.h"

const float WantedFreePopPercents = 20;


#define BUILD(X) PushBuildTask(pr--, X, Broodwar->self()->getStartLocation())
#define PROD(X) PushProductionTask(pr--, X);
static int pr = INT_MAX;


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
	BUILD(UnitTypes::Protoss_Gateway);
	PROD(UnitTypes::Protoss_Zealot);
	BUILD(UnitTypes::Protoss_Gateway);
	PROD(UnitTypes::Protoss_Zealot);
	PROD(UnitTypes::Protoss_Zealot);
	PROD(UnitTypes::Protoss_Zealot);
	PROD(UnitTypes::Protoss_Zealot);
	BUILD(UnitTypes::Protoss_Gateway);
	BUILD(UnitTypes::Protoss_Cybernetics_Core);
	BUILD(UnitTypes::Protoss_Assimilator);
	for (int i = 0; i < 5; ++i)
	{
		PROD(UnitTypes::Protoss_Zealot);
	}
	BUILD(UnitTypes::Protoss_Robotics_Facility);
	for (int i = 0; i < 4; ++i)
	{
		PROD(UnitTypes::Protoss_Dragoon);
		PROD(UnitTypes::Protoss_Zealot);
	}
	PushExpandTask(pr--);
	BUILD(UnitTypes::Protoss_Gateway);
	BUILD(UnitTypes::Protoss_Gateway);
	BUILD(UnitTypes::Protoss_Observatory);
	BUILD(UnitTypes::Protoss_Gateway);
	for (int i = 0; i <1100; ++i)
	{
		if (i % 45)
		{
			BUILD(UnitTypes::Protoss_Gateway);
			BUILD(UnitTypes::Protoss_Gateway);
			BUILD(UnitTypes::Protoss_Gateway);
			PushExpandTask(pr--);
		}
		if (i % 7 == 1) 
			//PROD(UnitTypes::Protoss_Observer);
		PROD(UnitTypes::Protoss_Zealot);
		PROD(UnitTypes::Protoss_Dragoon);
	}

	//PushBuildTask(2000, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	//PushProductionTask(1999, UnitTypes::Protoss_Zealot);
	//PushBuildTask(1998, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	//PushBuildTask(1997, UnitTypes::Protoss_Assimilator, Broodwar->self()->getStartLocation());
	//PushBuildTask(1996, UnitTypes::Protoss_Cybernetics_Core, Broodwar->self()->getStartLocation());
	//PushProductionTask(8, UnitTypes::Protoss_Zealot);
	//PushProductionTask(8, UnitTypes::Protoss_Zealot);
	//PushBuildTask(7, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	//PushBuildTask(7, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	//for (int i = 0; i < 2; ++i)
	//{
	//	PushProductionTask(6, UnitTypes::Protoss_Zealot);
	//	PushProductionTask(6, UnitTypes::Protoss_Dragoon);
	//}
	//PushBuildTask(5, UnitTypes::Protoss_Forge, Broodwar->self()->getStartLocation());
	//PushBuildTask(5, UnitTypes::Protoss_Gateway, Broodwar->self()->getStartLocation());
	//PushExpandTask(5);
	//for (int i = 0; i < 100; ++i)
	//{
	//	PushProductionTask(4, UnitTypes::Protoss_Zealot);
	//	PushProductionTask(4, UnitTypes::Protoss_Dragoon);
	//}
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
	size_t wantedSupply = std::max(static_cast<int>(Broodwar->self()->supplyTotal() * (WantedFreePopPercents / 100)), 6);
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

Position::list StrategyManager::GetExpandPos()
{
	Position::list res;
	for (auto& b : m_bases)
		res.push_back(b->getPosition());
	return res;
}
