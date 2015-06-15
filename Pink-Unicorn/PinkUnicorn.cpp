#include "PinkUnicorn.h"

#include "Utils.h"
#include "MapManager.h"
#include "ProductionManager.h"
#include "ResourceManager.h"
#include "WorkerManager.h"
#include "ProductionTasks.h"
#include "MacroManager.h"
#include "BuildingManager.h"

#include <iostream>

using namespace BWAPI;
using namespace Filter;

PinkUnicorn::PinkUnicorn()
{
	mManagers.push_back(&WorkerManager::GetInstance());
	mManagers.push_back(&ProduceManager::GetInstance());
	mManagers.push_back(&ResourceManager::GetInstance());
	mManagers.push_back(&BuildingManager::GetInstance());
	//mManagers.push_back(new MacroManager);
	//mManagers.push_back(&MapManager::GetInstance());
}


void PinkUnicorn::onFrame()
{
	
	//dispaly FPS
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());
	

	for (auto m : mManagers)
		m->OnFrame();

	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() ||
		Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;

	TaskQueue::GetInstance().ReleaseCompleteTasks();
	/*
	for (auto &u : Broodwar->self()->getUnits())
	{
		if (!u->exists())
			continue;
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;
		if (!u->isCompleted() || u->isConstructing())
			continue;
		if (u->getType().isWorker())
		{
			//if we got 9 probes
			if (Broodwar->self()->supplyUsed() >= 18 && !scouting)
			{
				std::cout << "Start scouting.\n";
				std::cout << "Map width: " << Broodwar->mapWidth() << std::endl;
				std::cout << "Map height: " << Broodwar->mapHeight() << std::endl;
				

				bool isFirst = false;
				for (auto& SL : Broodwar->getStartLocations())
				{
					if (!Broodwar->isExplored(SL.x, SL.y))
					{
						u->attack(Position(SL.x * 32 + 64, SL.y * 32 + 56), isFirst);
						isFirst = true;
					}
				}
				scouting = true;
			}
			// if our worker is idle
			if (u->isIdle())
			{
				if (u->isCarryingGas() || u->isCarryingMinerals())
				{
					u->returnCargo();
				}
				else if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
				{
					// If the call fails, then print the last error message
					Broodwar << Broodwar->getLastError() << std::endl;
				}
			}
		}
		// A resource depot is a Command Center, Nexus, or Hatchery
		else if (u->getType().isResourceDepot())
		{
			if (u->isIdle())
			{
				u->train(u->getType().getRace().getWorker());
			}
		}
	}
	*/
	if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() < 3)
	{
		BuildingManager::GetInstance().Build(UnitTypes::Protoss_Pylon);
	}

}


void PinkUnicorn::onStart()
{
	if (Broodwar->isReplay())
	{
		return;
	}
		
	Broodwar->sendText("Hello world!");
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
	Broodwar->enableFlag(Flag::UserInput);
	for (auto m : mManagers)
		m->OnStart();

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	//set game speed
	Broodwar->setLocalSpeed(0);
	//Broodwar->setFrameSkip(5);

}

void PinkUnicorn::onEnd(bool isWinner)
{
	if (isWinner)
		Broodwar << "I won the game" << std::endl;
	else
		Broodwar << "I lost the game" << std::endl;
}

void PinkUnicorn::onSendText(std::string text)
{
	if (Broodwar->isReplay())
	{
		return;
	}
	Broodwar->sendText("%s", text.c_str());
}

void PinkUnicorn::onReceiveText(BWAPI::Player player, std::string text)
{
	if (Broodwar->isReplay())
	{
		return;
	}
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void PinkUnicorn::onPlayerLeft(BWAPI::Player player)
{
	if (Broodwar->isReplay())
	{
		return;
	}
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void PinkUnicorn::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void PinkUnicorn::onUnitDiscover(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onUnitEvade(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onUnitShow(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
	if (Broodwar->self() != unit->getPlayer() && Broodwar->neutral() != unit->getPlayer())
		std::cout << "Enemy found: " << unit->getType().c_str() << std::endl;

}

void PinkUnicorn::onUnitHide(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onUnitCreate(BWAPI::Unit unit)
{
	if (Broodwar->getFrameCount() < 2)
		return;
	if (Broodwar->isReplay())
	{
		return;
	}
	if (unit->getType().isBuilding())//check isMine ?
	{
		BuildingManager::GetInstance().CheckNewBuildings(unit);
	}
}

void PinkUnicorn::onUnitDestroy(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
	
	for (auto m : mManagers)
		m->OnUnitDestroy(unit);
}

void PinkUnicorn::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onUnitRenegade(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onSaveGame(std::string gameName)
{

	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void PinkUnicorn::onUnitComplete(BWAPI::Unit unit)
{
	for (auto m : mManagers)
		m->OnUnitComplete(unit);

	if (Broodwar->isReplay())
	{
		return;
	}
	Broodwar << unit->getType().c_str() << std::endl;
}
