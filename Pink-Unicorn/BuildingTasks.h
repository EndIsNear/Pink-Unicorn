#ifndef PINK_UNICORN_BUILDING_TASKS_H
#define PINK_UNICORN_BUILDING_TASKS_H

#include "TasksQueue.h"

typedef std::auto_ptr<BWAPI::Position> PositionPtr;

class BuildingTask : public Task
{
	public:
		BuildingTask(BWAPI::UnitType Type, Task::Priority pri, BWAPI::Position *pos = NULL)
		{
			mPriority = pri;
			Task::mType = Task::Produce;
			mUnitType = Type;
			mpPosition = PositionPtr(new BWAPI::Position(*pos));
			mAllIsCommit = false;
			mExecutor = NULL;
		}


public:
	PositionPtr mpPosition;
	bool mAllIsCommit; // all sub tasks are  commit
	BWAPI::UnitType mUnitType;
	bool mbTryToReserved;
	BWAPI::Unit mExecutor;
};


#endif
