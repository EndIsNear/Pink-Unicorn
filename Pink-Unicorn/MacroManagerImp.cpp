#include "MacroManager.h"
#include "GameAnalyzer.h"
#include "WorkerManager.h"
#include "SpyManager.h"

using namespace BWAPI;

MacroManager *MacroManager::inst = NULL;


void MacroManager::OnStart()
{
	TilePosition pos = TilePositions::None;
	mMicroControler.SwitchTargetPoint(Position(MapManager::GetInstance().GetDefencePoint(Broodwar->self()->getStartLocation(), pos)));
	mMicroControler.SwitchTaskType(MicroControler::Defence);
}



inline double CalcScore1(Unitset &set)
{
	double result = 0;
	for (auto it : set)
	{
		if (IsBuilding(it) || IsWorker(it))
			continue;
		switch (it->getType())
		{

		default:
			result += it->getType().mineralPrice() + it->getType().gasPrice();
			break;
		}
	}
	return result;
}

void MacroManager::OnFrame()
{
	mMicroControler.OnFrame(1);

	if (Broodwar->getFrameCount() == 25 * 40)
	{
		Unit u;
		WorkerManager::GetInstance().ReleaseWorker(Position(Broodwar->self()->getStartLocation()), u);
		SpyManager::GetInstance().InitialSpy(u);
	}
	Unitset us = Broodwar->getUnitsInRadius(Position(Broodwar->self()->getStartLocation()), 1800, IsEnemy);
/*	if (us.size() > 5 && mMicroControler.GetType() != MicroControler::Defence)
	{
		TilePosition pos = TilePositions::None;
		mMicroControler.SwitchTargetPoint(Position(MapManager::GetInstance().GetDefencePoint(Broodwar->self()->getStartLocation(), pos)));
		mMicroControler.SwitchTaskType(MicroControler::Defence);

	}	else*/ 
	if (Broodwar->getFrameCount() % 150 )
	{

	
		if ((mMicroControler.GetSize() > 25 || mMicroControler.GetType() == MicroControler::Attack) )
		{
			auto eu = Broodwar->getUnitsInRadius(Position(SpyManager::GetInstance().GetEnemyBases().at(0)), 2500, IsEnemy);
			double EnemyScore = CalcScore1(eu);
			if (EnemyScore < mMicroControler.GetScore() * 0.85)
			{
				if (mMicroControler.GetType() != MicroControler::Attack)
				{
					mMicroControler.SwitchTargetPoint(Position(SpyManager::GetInstance().GetEnemyBases().back()));
					mMicroControler.SwitchTaskType(MicroControler::Attack);
				}
			}
			else if(mMicroControler.GetType() != MicroControler::Defence)
			{
				TilePosition pos = TilePositions::None;
				mMicroControler.SwitchTargetPoint(Position(MapManager::GetInstance().GetDefencePoints(Broodwar->self()->getStartLocation()).at(0)));
				mMicroControler.SwitchTaskType(MicroControler::Defence);
			}
		}/*/
		else	if (mMicroControler.GetSize() > 15 )
		{
			if (mMicroControler.GetType() != MicroControler::Presure)
			{
				mMicroControler.SwitchTargetPoint(Position(SpyManager::GetInstance().GetEnemyBases().at(0)));
				mMicroControler.SwitchTaskType(MicroControler::Presure);
			}
		}*/
		else if (mMicroControler.GetType() != MicroControler::Defence)
		{
			TilePosition pos = TilePositions::None;
			mMicroControler.SwitchTargetPoint(Position(MapManager::GetInstance().GetDefencePoints(Broodwar->self()->getStartLocation()).at(0)));
			mMicroControler.SwitchTaskType(MicroControler::Defence);
		}
	}


}
 
void MacroManager::OnUnitComplete(Unit u)
{
	if ( !IsWorker(u) && IsAlly(u))
	{
		mFreeUnits[u->getType()].insert(u);
		Unit worker; 
		if (Broodwar->getFrameCount() > 25 * 150 && mbSpyExpInit == false)
		{
			if (WorkerManager::GetInstance().ReleaseWorker(Position(Broodwar->self()->getStartLocation()), worker))
			{
				SpyManager::GetInstance().ExploreLocations(worker, MapManager::GetInstance().GetExpansionLocations());
				mbSpyExpInit = true;
			}
		}

		if ((mFreeUnits[UnitTypes::Protoss_Zealot].size() || mFreeUnits[UnitTypes::Protoss_Dragoon].size()))
		{
			
			for (auto it : mFreeUnits[UnitTypes::Protoss_Dragoon])
				mMicroControler.AddUnit(it);
			for (auto it : mFreeUnits[UnitTypes::Protoss_Zealot])
				mMicroControler.AddUnit(it);
			for (auto it : mFreeUnits[UnitTypes::Protoss_Observer])
				mMicroControler.AddUnit(it);
			mFreeUnits[UnitTypes::Protoss_Zealot].clear();
			mFreeUnits[UnitTypes::Protoss_Dragoon].clear();


		}

		
	}
}

void MacroManager::OnUnitDestroy(Unit u)
{
	Unitset& set = mFreeUnits[u->getType()];
	if (set.contains(u))
	{
		set.erase(u);
	}
}

int MacroManager::GetUnitsCntInConrolers(UnitType type) const
{
	Unitset uset;
//	MicroControler.GetUnitsWithType(uset, type);
	return uset.size();
}
