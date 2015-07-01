#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H
#include "Agent.h"
#include "ManagerBase.h"
#include "map"


class MacroManager : public ManagerBase
{
	public:
		MacroManager() {}
		virtual void OnFrame() override;
		virtual void OnUnitComplete(Unit u) override;
		virtual void OnUnitDestroy(Unit u) override;
		int getUnitCnt(UnitType type){ return mFreeUnits[type].size() + GetUnitsCntInConrolers(type);}
		void AddUnit(Unit u) { mFreeUnits[u->getType()].insert(u);}
	private:
		int GetUnitsCntInConrolers(UnitType type) const ;
	private:
		std::map<UnitType, Unitset> mFreeUnits;
		std::vector<MicroControler> MicroControlers;
};


#endif