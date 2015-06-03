#ifndef PINK_UNICORN_MANAGER_BASE_H
#define PINK_UNICORN_MANAGER_BASE_H

class ManagerBase 
{
	public:
	virtual void OnFrame() = 0;
	virtual void CheckForNewTask() = 0;

};

#endif