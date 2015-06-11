#include "GameAnalyzer.h"
#include "TasksQueue.h"
#include "AllTasks.h"

using  namespace BWAPI;
using  namespace Filter;

const int FramesInMinute = 25 * 60;
GameAnalyzer*  GameAnalyzer::inst = NULL;


double GameAnalyzer::GetResourceProductionValue()
{
	double workersCnt = Broodwar->self()->getUnits().getUnitsInRadius(1000000, IsWorker).size();
	workersCnt += GetWorkersInProductionTasks();
	if (Broodwar->getFrameCount() < FramesInMinute * 5) // early game
		return workersCnt / 20.;
	else if (Broodwar->getFrameCount() < FramesInMinute * 15)
		return workersCnt / 45;
	else
		return workersCnt / 60;
}

double GameAnalyzer::GetUnitProductionValue()
{
	return 1.; // to do implement
}

double GameAnalyzer::GetArmyValue()
{
	double zelotsCnt = Broodwar->self()->getUnits().getUnitsInRadius(1000000, GetType == UnitTypes::Protoss_Zealot).size();
	double dragoonsCnt = Broodwar->self()->getUnits().getUnitsInRadius(1000000, GetType == UnitTypes::Protoss_Dragoon).size();
	double pureVal =  zelotsCnt * 100. + dragoonsCnt * 150.;
	double BalanceKoef = 1 - (zelotsCnt*0.8) / dragoonsCnt; 
	return pureVal * BalanceKoef;
}

double GameAnalyzer::GetMapControlValue()
{
	return 1.;
}

const double maxPosibleSypply = 200.;
double GameAnalyzer::GetSupplyValue()
{
	double currTotal = Broodwar->self()->supplyTotal();
	double currUsed = Broodwar->self()->supplyTotal();
	return currTotal / currUsed - std::max(currTotal / maxPosibleSypply, 0.15);// max  15 % advance
}

int GameAnalyzer::GetWorkersInProductionTasks()
{
	TaskList l;
	TaskQueue::GetInstance().GetTasksWithType(Task::Produce, l, false);
	int cnt = 0;
	for (auto it : l)
	{
		if (SingleUnitProduction *temp = dynamic_cast<SingleUnitProduction*>(it.get()))
			if (temp->mUnitType.isWorker())
			cnt++;
	}
	return cnt;
}

