#ifndef PINK_UNICORN_BUILDING_MANAGER_H
#define PINK_UNICORN_BUILDING_MANAGER_H

#include "ManagerBase.h"

class BuildingManager : public ManagerBase
{
	public:
		BuildingManager(){};
		virtual void OnFrame() override;
		void ExecuteBuildTask(BuildingTask &task);
};

#endif