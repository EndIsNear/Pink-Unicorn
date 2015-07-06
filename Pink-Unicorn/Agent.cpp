#include "Agent.h"
#include "SpyManager.h"
//#include "AgentsFilters.h"


DragoonControl::DragoonControl(Unit drag) : ControlPattern(drag)
{
	
	//Agents.push_back(new AssessTheEnemy(drag, 300, IsEnemy && !IsBuilding));
	// this must be fix but for now i ok 
	Agents.push_back(new AgentStayAway(drag, 75, 50, IsEnemy && !IsBuilding));
	Agents.push_back(new AgentAttackInRange(drag, 250, 150, IsEnemy && !IsBuilding));
	//Agents.push_back(new AgentStayAway(drag, 10, 5, IsAlly));
	//Agents.push_back(new AgentStayToghether(drag, 300));
	Agents.push_back(new AgentGoToPosition(drag, Position(SpyManager::GetInstance().GetEnemyBases()[0])));
}


ZelotControl::ZelotControl(Unit zelka) : ControlPattern(zelka)
{
	//	Agents.push_back(new AssessTheEnemy(zelka, 200));
	
	Agents.push_back(new AgentAttackInRange(zelka, 100, 10, IsEnemy && !IsBuilding));
	//	Agents.push_back(new AgentStayToghether(zelka, 300));
	Agents.push_back(new AgentGoToPosition(zelka, Position(SpyManager::GetInstance().GetEnemyBases()[0])));
	//Agents.push_back(new AgentStayToghether(zelka, 500));
}