#ifndef PINK_UNICORN_MANAGER_BASE_H
#define PINK_UNICORN_MANAGER_BASE_H

#include <BWAPI.h>
#include <BWAPI/Client.h>

class ManagerBase 
{
	public:
	virtual void OnFrame() = 0;
	virtual void CheckForNewTask() = 0;

};

#endif