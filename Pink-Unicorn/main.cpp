#include <BWAPI.h>
#include <BWAPI/Client.h>


#include <iostream>
#include <thread>
#include <chrono>
#include <string>


#include "PinkUnicorn.h"
#include "Utils.h"

using namespace BWAPI;

bool show_bullets;
bool show_visibility_data;

void reconnect()
{
	while (!BWAPIClient.connect())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds{ 1000 });
	}
}

int main(int argc, const char* argv[])
{
	std::cout << "Connecting..." << std::endl;;
	reconnect();
	while (true)
	{
		std::cout << "waiting to enter match" << std::endl;
		while (!Broodwar->isInGame())
		{
			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				std::cout << "Reconnecting..." << std::endl;;
				reconnect();
			}
		}
		if (Broodwar->isReplay())
		{
			return 0;
		}

		PinkUnicorn aiModule;

		while (Broodwar->isInGame())
		{
			for (auto &e : Broodwar->getEvents())
			{
				switch (e.getType())
				{
				case EventType::MatchStart:
					aiModule.onStart();
					break;
				case EventType::MatchFrame:
					aiModule.onFrame();
					break;
				case EventType::MatchEnd:
					aiModule.onEnd(e.isWinner());
					break;
				case EventType::SendText:
					aiModule.onSendText(e.getText());
					break;
				case EventType::ReceiveText:
					aiModule.onReceiveText(e.getPlayer(), e.getText());
					break;
				case EventType::PlayerLeft:
					aiModule.onPlayerLeft(e.getPlayer());
					break;
				case EventType::NukeDetect:
					aiModule.onNukeDetect(e.getPosition());
					break;
				case EventType::UnitCreate:
					aiModule.onUnitCreate(e.getUnit());
					break;
				case EventType::UnitDestroy:
					aiModule.onUnitDestroy(e.getUnit());
					break;
				case EventType::UnitMorph:
					aiModule.onUnitMorph(e.getUnit());
					break;
				case EventType::UnitShow:
					aiModule.onUnitShow(e.getUnit());
					break;
				case EventType::UnitHide:
					aiModule.onUnitHide(e.getUnit());
					break;
				case EventType::UnitRenegade:
					aiModule.onUnitRenegade(e.getUnit());
					break;
				case EventType::SaveGame:
					aiModule.onSaveGame(e.getText());
					break;
				case EventType::UnitComplete:
					aiModule.onUnitComplete(e.getUnit());
					break;
				}
			}

			drawStartPos();
			drawRegions(0);
			drawStats();
			//drawBullets();
			//drawVisibilityData();

			BWAPI::BWAPIClient.update();
			if (!BWAPI::BWAPIClient.isConnected())
			{
				std::cout << "Reconnecting..." << std::endl;
				reconnect();
			}
		}

		std::cout << "Game ended" << std::endl;
	}
	std::cout << "Press ENTER to continue..." << std::endl;
	std::cin.ignore();
	return 0;
}
