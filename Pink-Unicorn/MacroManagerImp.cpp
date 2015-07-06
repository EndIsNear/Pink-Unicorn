#include "MacroManager.h"
#include "GameAnalyzer.h"
#include "WorkerManager.h"
#include "SpyManager.h"

using namespace BWAPI;

MacroManager *MacroManager::inst = NULL;

void MacroManager::OnFrame()
{
	MicroControler.OnFrame(1);

	if (Broodwar->getFrameCount() == 25 * 40)
	{
		Unit u;
		WorkerManager::GetInstance().ReleaseWorker(Position(Broodwar->self()->getStartLocation()), u);
		SpyManager::GetInstance().InitialSpy(u);
	}
}
 
void MacroManager::OnUnitComplete(Unit u)
{
	if ( !IsWorker(u) && IsAlly(u))
	{
		mFreeUnits[u->getType()].insert(u);
		Unit worker; 
		if (Broodwar->getFrameCount() > 25 * 60 && mbSpyExpInit == false)
		{
			if (WorkerManager::GetInstance().ReleaseWorker(Position(Broodwar->self()->getStartLocation()), worker))
			{
				SpyManager::GetInstance().ExploreLocations(worker, MapManager::GetInstance().GetExpansionLocations());
				mbSpyExpInit = true;
			}
		}

		auto BaseDefPos = MapManager::GetInstance().GetDefencePoints(Broodwar->self()->getStartLocation());

		if (SpyManager::GetInstance().GetEnemyBases().size())
		{
			if ((mFreeUnits[UnitTypes::Protoss_Zealot].size() || mFreeUnits[UnitTypes::Protoss_Dragoon].size()))
			{
				for (auto it : mFreeUnits[UnitTypes::Protoss_Dragoon])
					MicroControler.AddUnit(it, Position(BaseDefPos.at(0)));
				for (auto it : mFreeUnits[UnitTypes::Protoss_Zealot])
					MicroControler.AddUnit(it, Position(BaseDefPos.at(0)));
				mFreeUnits[UnitTypes::Protoss_Zealot].clear();
				mFreeUnits[UnitTypes::Protoss_Dragoon].clear();
			}
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
