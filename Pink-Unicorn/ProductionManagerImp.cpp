#include "ManagerBase.h"
#include "WorkerManager.h"
#include "ResourceManager.h"

#include "ProductionManager.h"

ProduceManager * ProduceManager::m_instance = NULL;

void ProduceManager::OnFrame()
{

}

void ProduceManager::OnUnitComplete(BWAPI::Unit unit)
{
	if (Broodwar->getFrameCount() < 1)
		return;
	if (unit->getType().isWorker() && unit->isCompleted() || unit->getType() == UnitTypes::Protoss_Assimilator)
		WorkerManager::GetInstance().AddUnit(unit);
	//else if (!unit->getType().isBuilding())
	//	MacroManager::GetInstance().AddUnit(unit);
}

void ProduceManager::ProduceSingleUnit(UnitType unit)
{

}

void ProduceManager::ProduceSingleUnitFrom(UnitType unit, Unit producer)
{
	if (ResourceManager::GetInstance().ReserveRes(unit.mineralPrice(), unit.gasPrice(), unit.supplyRequired()))
	{
		if (unit.whatBuilds().first == producer->getType() && producer->isIdle())
		{
			producer->train(unit);
		}
		ResourceManager::GetInstance().ReleaseRes(unit.mineralPrice(), unit.gasPrice(), unit.supplyRequired());
	}
	
}
