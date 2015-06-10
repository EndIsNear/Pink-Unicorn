#include "MacroManager.h"
#include "GameAnalyzer.h"
#include "AllTasks.h"
#include "TasksQueue.h"
using namespace BWAPI;

void MacroManager::OnFrame()
{
	GameAnalyzer &ga = GameAnalyzer::GetInstance();

	if (ga.GetResourceProductionValue() < 1.)
	{
		TaskQueue::GetInstance().push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Probe, Task::MAX)));
	}
	//if (ga.GetMapControlValue() < 1.)
		//TaskQueue::GetInstance().push_back(TaskPtr(new SpyTask)
	if (ga.GetArmyValue() < 10000.)
	{
		TaskQueue::GetInstance().push_back(TaskPtr(new SingleUnitProduction(BWAPI::UnitTypes::Protoss_Zealot, Task::MAX)));
	}
}