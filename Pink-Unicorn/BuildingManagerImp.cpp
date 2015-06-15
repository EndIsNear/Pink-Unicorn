#include "ManagerBase.h"
#include "WorkerManager.h"
#include "MapManager.h"
#include "ResourceManager.h"

#include "BuildingManager.h"

BuildingManager * BuildingManager::m_instance = NULL;

void BuildingManager::OnFrame()
{

}

bool BuildingManager::BuildBaseExit(UnitType building)
{
	TilePosition pos = (Broodwar->self()->getStartLocation() + TilePosition(MapManager::GetInstance().GetBaseExit())) / 2;
	return BuildNearTo(building, pos);

}

bool BuildingManager::Build(UnitType building)
{
	return BuildNearTo(building, Broodwar->self()->getStartLocation());
}

bool BuildingManager::BuildNearTo(UnitType building, TilePosition pos)
{
	if (building.isBuilding() && Broodwar->canMake(building))
	{
		if (ResourceManager::GetInstance().ReserveRes(building.mineralPrice(), building.gasPrice(), 0))
		{
			Unit worker;
			if (WorkerManager::GetInstance().ReleaseWorker(Position(), worker))
			{
				TilePosition buildPos = MapManager::GetInstance().SuggestBuildingLocation(UnitTypes::Protoss_Pylon, pos);
				if (Broodwar->canBuildHere(buildPos, building, worker))
				{
					worker->build(building, buildPos);
					if (building == UnitTypes::Protoss_Pylon)
						m_SupplyInProgress += SupplyPerPylon;
					m_BuildingsInProgress.push_back(BuildingPair(building, worker));
				}
			}
		}
	}
	return false;
}

void BuildingManager::CheckNewBuildings(Unit building)
{
	for (size_t i = 0; i < m_BuildingsInProgress.size(); ++i)
	{
		if (building->getType() == m_BuildingsInProgress[i].first 
			&& building->getDistance(m_BuildingsInProgress[i].second) < 32)
		{
			WorkerManager::GetInstance().AddUnit(m_BuildingsInProgress[i].second);
			ResourceManager::GetInstance().ReleaseRes(building->getType().mineralPrice(), building->getType().gasPrice(), 0);
			m_BuildingsInProgress.erase(m_BuildingsInProgress.begin() + i);
		}
	}
}
