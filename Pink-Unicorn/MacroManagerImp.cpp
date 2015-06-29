#include "MacroManager.h"
#include "GameAnalyzer.h"

using namespace BWAPI;

void MacroManager::OnFrame()
{
	for (auto it : MicroControlers)
	{
		it.OnFrame(1);
	}
}


void MacroManager::OnUnitComplete(Unit u)
{
	mFreeUnits[u->getType()].insert(u);
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