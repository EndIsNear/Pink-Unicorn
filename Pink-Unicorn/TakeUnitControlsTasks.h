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
	TakeUnitTask(BWAPI::Unit &u/*this is pointer but just in case*/) : unit(u)
	{
		mPriority = Task::MAX;
		mType = Task::TakeControl;
		mIsComplete = false;
	}
public:
	BWAPI::Unit unit;
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

#endif