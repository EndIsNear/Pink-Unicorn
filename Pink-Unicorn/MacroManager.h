#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "ManagerBase.h"
class MacroManager : public ManagerBase
{
	public:
		MacroManager() {}
		virtual void OnFrame() override;
	private:
		TaskList myTasks;
};


#endif