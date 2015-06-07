#include "PinkUnicorn.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

bool scouting = false;

void PinkUnicorn::onFrame()
{
	
	//dispaly FPS
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self() ||
		Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;
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
		//m_WorkerManager->Refresh();
	}
	*/
}


void PinkUnicorn::onStart()
{
	Broodwar->sendText("Hello world!");
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;
	Broodwar->enableFlag(Flag::UserInput);
	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	m_SupplyProviderType = Broodwar->self()->getRace().getSupplyProvider();
	Broodwar->setCommandOptimizationLevel(2);

	/*auto start = Broodwar->self()->getStartLocation();
	//std::cout << start;
	auto nexpos = MapManager::GetInstance().SuggestBuildingLocation(UnitTypes::Protoss_Nexus);
	//std::cout << nexpos;
	auto nexreg = Broodwar->getRegionAt(Position(nexpos));
	auto center = nexreg->getCenter();
	std::cout << nexreg->getID();
	Broodwar->drawCircle(CoordinateType::Map, center.x, center.y, 12, Colors::Red, true);
	Broodwar->drawTextMap(nexreg->getCenter(), "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAaa");*/

	// Check if this is a replay
	if (Broodwar->isReplay())
	{
		return;
	}
	else
	{
		if (Broodwar->enemy()) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}

	//set game speed
	Broodwar->setLocalSpeed(1);
	//->setFrameSkip(5);
}

void PinkUnicorn::onEnd(bool isWinner)
{
	if (isWinner)
		Broodwar << "I won the game" << std::endl;
	else
		Broodwar << "I lost the game" << std::endl;
}

void onFrame2()
{
	//// Iterate through all the units that we own
	//for (auto &u : Broodwar->self()->getUnits())
	//{
	//	else if (u->getType().isResourceDepot()) // A resource depot is a Command Center, Nexus, or Hatchery
	//	{
	//		// Order the depot to construct more workers! But only when it is idle.
	//		if (u->isIdle() && !u->train(u->getType().getRace().getWorker()))
	//		{
	//			// If that fails, draw the error at the location so that you can visibly see what went wrong!
	//			// However, drawing the error once will only appear for a single frame
	//			// so create an event that keeps it on the screen for some frames
	//			Position pos = u->getPosition();
	//			//Error lastErr = Broodwar->getLastError();
	//			//Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
	//			//	nullptr,    // condition
	//			//	Broodwar->getLatencyFrames());  // frames to run

	//			// Retrieve the supply provider type in the case that we have run out of supplies
	//			static int lastChecked = 0;

	//			// If we are supply blocked and haven't tried constructing more recently
	//			if (Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() < 2 &&
	//				lastChecked + 400 < Broodwar->getFrameCount() &&
	//				Broodwar->self()->incompleteUnitCount(m_SupplyProviderType) == 0)
	//			{
	//				lastChecked = Broodwar->getFrameCount();

	//				// Retrieve a unit that is capable of constructing the supply needed
	//				Unit supplyBuilder = u->getClosestUnit(GetType == m_SupplyProviderType.whatBuilds().first &&
	//					(IsIdle || IsGatheringMinerals) &&
	//					IsOwned);
	//				// If a unit was found
	//				if (supplyBuilder)
	//				{
	//					if (m_SupplyProviderType.isBuilding())
	//					{
	//						TilePosition targetBuildLocation = Broodwar->getBuildLocation(m_SupplyProviderType, supplyBuilder->getTilePosition());
	//						UnitType supplyProviderType = supplyProviderType;
	//						if (targetBuildLocation)
	//						{
	//							// Register an event that draws the target build location
	//							Broodwar->registerEvent([targetBuildLocation, supplyProviderType](Game*)
	//							{
	//								Broodwar->drawBoxMap(Position(targetBuildLocation),
	//									Position(targetBuildLocation + supplyProviderType.tileSize()),
	//									Colors::Blue);
	//							},
	//								nullptr,  // condition
	//								m_SupplyProviderType.buildTime() + 100);  // frames to run

	//							// Order the builder to construct the supply structure
	//							supplyBuilder->build(m_SupplyProviderType, targetBuildLocation);
	//						}
	//					}
	//					else
	//					{
	//						// Train the supply provider (Overlord) if the provider is not a structure
	//						supplyBuilder->train(m_SupplyProviderType);
	//					}
	//				} // closure: supplyBuilder is valid
	//			} // closure: insufficient supply
	//		} // closure: failed to train idle unit

	//	}

	//} // closure: unit iterator
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
	if (Broodwar->isReplay())
	{
		return;
	}
}

void PinkUnicorn::onUnitDestroy(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		return;
	}
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
	if (Broodwar->isReplay())
	{
		return;
	}
	Broodwar << unit->getType().c_str() << std::endl;
}
