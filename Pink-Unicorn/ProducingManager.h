#ifndef PINK_UNICORN_PRODUCE_MANAGER_H
#include PINK_UNICORN_PRODUCE_MANAGER_H

#include "ManagerBase.h"

class ProduceManager 
{
	
	public:
		virtual void OnFrame() override {};
		virtual void CheckForNewTask() override{};

};

#endif