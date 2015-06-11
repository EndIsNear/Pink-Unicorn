#ifndef PINK_UNICORN_MANAGER_BASE_H
#define PINK_UNICORN_MANAGER_BASE_H

#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "TasksQueue.h"
#include "AllTasks.h"

class ManagerBase 
{
public:
	virtual void OnFrame() = 0;
	virtual void CheckForNewTask() = 0;
	virtual void OnUnitComplete(BWAPI::Unit unit){}
};


typedef std::vector<ManagerBase*> ManagersCollection;
#endif