#include "ManagerBase.h"
#include "WorkerManager.h"
#include "MapManager.h"

#include "BuildingManager.h"

BuildingManager * BuildingManager::m_instance = NULL;

void BuildingManager::OnFrame()
{

}

bool BuildingManager::Build(UnitType building)
{
	if (building.isBuilding() && Broodwar->canMake(building))
	{
		Unit worker;
		if (WorkerManager::GetInstance().ReleaseWorker(Position(), worker))
		{
			TilePosition buildPos = MapManager::GetInstance().SuggestBuildingLocation(UnitTypes::Protoss_Pylon);
			if (Broodwar->canBuildHere(buildPos, building, worker))
			{
				worker->build(building, buildPos);
				m_BuildingsInProgress.push_back(BuildingPair(building, worker));
			}
		}
	}
	return false;
}

void BuildingManager::CheckNewBuildings(Unit building)
{
	for (size_t i = 0; i < m_BuildingsInProgress.size(); ++i)
	{
		if (building->getType() == m_BuildingsInProgress[i].first)// && m_BuildingsInProgress[i].second->isIdle())
		{
			WorkerManager::GetInstance().AddUnit(m_BuildingsInProgress[i].second);
			m_BuildingsInProgress.erase(m_BuildingsInProgress.begin() + i);
		}
	}
}
