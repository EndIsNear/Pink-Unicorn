#include "ManagerBase.h"
#include "WorkerManager.h"
#include "MapManager.h"
#include "ResourceManager.h"
#include "StrategyManager.h"

#include "BuildingManager.h"

BuildingManager * BuildingManager::m_instance = NULL;

void BuildingManager::OnFrame()
{

}

void BuildingManager::OnUnitComplete(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Pylon)
		m_SupplyInProgress -= SupplyPerPylon;
	else if (unit->getType() == UnitTypes::Protoss_Nexus && Broodwar->getFrameCount() > 100)
	{
		WorkerManager::GetInstance().AddUnit(unit);
		StrategyManager::GetInstance().AddBase(unit);
	}
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
			if (WorkerManager::GetInstance().ReleaseWorker(Position(), worker) && worker->hasPath(Position(pos)))
			{
				if (Broodwar->isExplored(pos))
				{
					return BuildWithNearTo(worker, building, pos);
				}
				else
				{
					pos.x -= 1;//move away from build pos
					worker->move(Position(pos));
					pos.x += 1;//return to the right build position
					Broodwar->registerEvent(
						[=](Game * p)
						{
							if (worker->exists())
								BuildingManager::GetInstance().BuildWithNearTo(worker, building, pos);
							else
								ResourceManager::GetInstance().ReleaseRes(building.mineralPrice(), building.gasPrice(), 0);
						},
						[worker, pos](Game * p){ return worker->isIdle() && Broodwar->isExplored(pos); },
						1, 20
						);
					return true;
				}
			}
			else
			{
				ResourceManager::GetInstance().ReleaseRes(building.mineralPrice(), building.gasPrice(), 0);
			}
		}
	}
	return false;
}

bool BuildingManager::BuildWithNearTo(Unit builder, UnitType building, TilePosition pos)
{
	TilePosition buildPos = MapManager::GetInstance().SuggestBuildingLocation(building, pos);
	if (builder->build(building, buildPos))
	{
		if (building == UnitTypes::Protoss_Pylon)
			m_SupplyInProgress += SupplyPerPylon;
		m_BuildingsInProgress.push_back(BuildingPair(building, builder));
		return true;
	}
	else
	{
		WorkerManager::GetInstance().AddUnit(builder);
		ResourceManager::GetInstance().ReleaseRes(building.mineralPrice(), building.gasPrice(), 0);
	}
	return false;
}

void BuildingManager::CheckNewBuildings(Unit building)
{
	for (size_t i = 0; i < m_BuildingsInProgress.size(); ++i)
	{
		if (building->getType() == m_BuildingsInProgress[i].first 
			&& building->getDistance(m_BuildingsInProgress[i].second) < 64)
		{
			WorkerManager::GetInstance().AddUnit(m_BuildingsInProgress[i].second);
			ResourceManager::GetInstance().ReleaseRes(building->getType().mineralPrice(), building->getType().gasPrice(), 0);
			m_BuildingsInProgress.erase(m_BuildingsInProgress.begin() + i);
		}
	}
}
