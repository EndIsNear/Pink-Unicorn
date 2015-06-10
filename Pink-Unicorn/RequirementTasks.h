#ifndef PINK_UNICORN_REQUIREMENT_TASKS_H
#define PINK_UNICORN_REQUIREMENT_TASKS_H

#include "TasksQueue.h"
#include <BWAPI.h>
#include <BWAPI/Client.h>

class RequireBuilding : public Task
{
	public:
		RequireBuilding(BWAPI::UnitType buildingType, Task::Priority pri)
		{
			Task::mType = Task::Requirement;
			Task::mPriority = pri;
			mBuildingType = buildingType;
		}

	public:
		BWAPI::UnitType mBuildingType;
};

class RequireTech : public Task
{

};

#endif