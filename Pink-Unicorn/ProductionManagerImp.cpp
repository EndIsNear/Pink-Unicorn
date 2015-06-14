#include "ManagerBase.h"
#include "WorkerManager.h"

#include "ProductionManager.h"

ProduceManager * ProduceManager::m_instance = NULL;

void ProduceManager::OnFrame()
{

}

void ProduceManager::OnUnitComplete(BWAPI::Unit unit)
{
	if (Broodwar->getFrameCount() < 1)
		return;
	if (unit->getType().isWorker() && unit->isCompleted())
		WorkerManager::GetInstance().AddUnit(unit);
	//else if (!unit->getType().isBuilding())
	//	MacroManager::GetInstance().AddUnit(unit);
}

void ProduceManager::ProduceSingleUnit(UnitType unit)
{

}

void ProduceManager::ProduceSingleUnitFrom(UnitType unit, Unit producer)
{
	if (unit.whatBuilds().first == producer->getType() && producer->isIdle())
	{
		producer->train(unit);
	}
	
}
