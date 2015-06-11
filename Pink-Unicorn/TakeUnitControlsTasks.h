#ifndef PINK_UNICORN_TAKE_UNIT_CONTROL_TASKS
#define PINK_UNICORN_TAKE_UNIT_CONTROL_TASKS

#include <BWAPI.h>
#include <BWAPI/Client.h>
#include <list>

#include "TasksQueue.h"
using namespace BWAPI;

class TakeUnitTask : public Task
{
public:
	TakeUnitTask(BWAPI::Unit &u/*this is pointer but just in case*/) : mUnit(u)
	{
		mPriority = Task::MAX;
		mType = Task::TakeControl;
	}
public:
	BWAPI::Unit mUnit;
};

class TakeWorkerTask : public TakeUnitTask
{
	public:
		TakeWorkerTask(BWAPI::Unit &u) :TakeUnitTask(u){}
};

class TakeArmyUnitTask : public TakeUnitTask
{
	public:
		TakeArmyUnitTask(BWAPI::Unit &u) :TakeUnitTask(u){}
};

class GetUnitTask : public Task
{
	public:
		GetUnitTask(BWAPI::UnitType unitType, BWAPI::Unit		*pUnit, Task::Priority pri)
		{
			mPriority = pri;
			mType = Task::GetControl;
			mIsComplete = false;
			mUnitType = unitType;
			mpUnit = pUnit;
		}
		BWAPI::UnitType mUnitType;
		BWAPI::Unit		*mpUnit; 
};

#endif