#ifndef PINK_UNICORN_PRODUCE_MANAGER_H
#define PINK_UNICORN_PRODUCE_MANAGER_H

#include "ManagerBase.h"

class ProduceManager : public ManagerBase
{
	
	public:
		virtual void OnFrame() override ;
		virtual void CheckForNewTask() override{};

	private:
	std::list<Task*> mCurrentTasks;
};


#endif