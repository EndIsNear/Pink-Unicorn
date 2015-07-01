#include "MacroManager.h"
#include "GameAnalyzer.h"
#include "WorkerManager.h"
#include "SpyManager.h"

using namespace BWAPI;

MacroManager *MacroManager::inst = NULL;

void MacroManager::OnFrame()
{
	for (auto it : MicroControlers)
	{
		it.OnFrame(1);
	}

	if (Broodwar->getFrameCount() == 25 * 40)
	{
		Unit u;
		WorkerManager::GetInstance().ReleaseWorker(Position(Broodwar->self()->getStartLocation()), u);
		SpyManager::GetInstance().InitialSpy(u);
	}
}


void MacroManager::OnUnitComplete(Unit u)
{
	if (IsWorker(u) == false && !IsEnemy(u))
	{
		mFreeUnits[u->getType()].insert(u);

		if (SpyManager::GetInstance().GetEnemyBases().size())
		{
			if (MicroControlers.size() && (mFreeUnits[UnitTypes::Protoss_Zealot].size() || mFreeUnits[UnitTypes::Protoss_Dragoon].size()))
			{
				for (auto it : mFreeUnits[UnitTypes::Protoss_Dragoon])
					MicroControlers[0].AddUnit(it);
				for (auto it : mFreeUnits[UnitTypes::Protoss_Zealot])
					MicroControlers[0].AddUnit(it);
				mFreeUnits[UnitTypes::Protoss_Zealot].clear();
				mFreeUnits[UnitTypes::Protoss_Dragoon].clear();
			}
			else if ((mFreeUnits[UnitTypes::Protoss_Zealot].size() + mFreeUnits[UnitTypes::Protoss_Dragoon].size())> 2 && MicroControlers.size() == 0)
			{
				MicroControlers.push_back(MicroControler());
				for (auto it : mFreeUnits[UnitTypes::Protoss_Dragoon])
					MicroControlers[0].AddUnit(it);
				for (auto it : mFreeUnits[UnitTypes::Protoss_Zealot])
					MicroControlers[0].AddUnit(it);
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
	for (auto it : MicroControlers)
		it.GetUnitsWithType(uset, type);
	return uset.size();
}
