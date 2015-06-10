#ifndef MACRO_MANAGER_H
#define MACRO_MANAGER_H

#include "ManagerBase.h"
#include "AllTasks.h"
class MacroManager : public ManagerBase
{
	public:
		MacroManager() {}
		virtual void OnFrame() override;
	private:
		void ExecuteRequireBuildinTask(RequireBuilding &task);
		TaskList myTasks;
};


#endif