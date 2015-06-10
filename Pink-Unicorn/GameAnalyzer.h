#ifndef UNICORN_GAME_ANALYZER_H
#define UNICORN_GAME_ANALYZER_H

#include <BWAPI.h>
#include <BWAPI/Client.h>
class GameAnalyzer
{
	GameAnalyzer(){};
	public:
		static GameAnalyzer& GetInstance()
		{
			if (inst)
				inst = new GameAnalyzer;
			return *inst;
		}

		double GetResourceProductionValue();
		double GetUnitProductionValue();
		double GetArmyValue();
		double GetMapControlValue();
		double GetSupplyValue();
	private:
		int GetWorkersInProductionTasks() ;
private:
	static  GameAnalyzer *inst;
	
};

#endif