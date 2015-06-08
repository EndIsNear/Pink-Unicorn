#ifndef PINK_UNICORN_PRODUCTION_TASKS_H
#define PINK_UNICORN_PRODUCTION_TASKS_H

#include <BWAPI.h>
#include <BWAPI/Client.h>
#include <list>

#include "TasksQueue.h"

class SingleUnitProduction : public Task
{
	public:
		SingleUnitProduction(BWAPI::UnitType &Type, Task::Priority pri)
		{
			mPriority = pri;
			Task::mType = Task::Produce;
			mType = Type;
			mTimeLimit = -1;
			mIsDenied = false;
			mIsComplete = false;
			mCallBack = NULL;
		}


	public:
		BWAPI::UnitType mType;
};




struct ProductionPair
{
	BWAPI::UnitType type;
	unsigned count;
};

typedef std::list<ProductionPair> ProductionList;
/*
class UnitsProductionTask : public Task
{
public:
	UnitProductionTask(ProductionList &l, Task::Priority pri);
	void Execute();
	void OnFrame();

	ProductionList mProductionList;
};

UnitProductionTask::UnitProductionTask(ProductionList &l, Task::Priority pri)
:
mProductionList(l)
{
	mPriority = pri;
	mType = Task::Produce;
	mTimeLimit = -1;
	isDenied = false;
	isComplete = false;
	mCallBack = NULL;
}

void ProductionManager::OnFrame()
{
	}
	*/
#endif