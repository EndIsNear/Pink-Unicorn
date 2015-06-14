#include "ProductionManager.h"
#include "ResourceTasks.h"
#include "DebugStuff.h"
#include "TakeUnitControlsTasks.h"

using namespace BWAPI;
using namespace Filter;

void ProduceManager::OnFrame()
{

}

void ProduceManager::OnUnitComplete(BWAPI::Unit unit)
{
	if (Broodwar->getFrameCount() < 2)
		return;
}
