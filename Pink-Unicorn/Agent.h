#ifndef PINK_UNICORN_AGENT_H
#define PINK_UNICORN_AGENT_H


#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "ManagerBase.h"
#include "2dVector.h"

using namespace BWAPI;
using namespace Filter;


class Agent
{
public:
	Agent(Unit u) : mUnit(u) { }
	virtual ~Agent(){}
	virtual void OnDraw() {}
	virtual bool OnFrame() = 0;
protected:
	Unit mUnit;
};

inline Unit LowHP(Unitset us)
{
	Unit Result = *us.begin();
	for (auto it : us)
	{
		if (Result->getHitPoints() > it->getHitPoints())
			Result = it;
	}
	return Result;
}

class DoSomeThingInRange
{
protected:
	DoSomeThingInRange(int dist, PtrUnitFilter filter) :mDist(dist), mFilter(filter) {}
protected:
	PtrUnitFilter mFilter;
	int mDist;
};


class AgentStayAway : public Agent, public DoSomeThingInRange
{
public:
	AgentStayAway(Unit u, int dist, double moveWithDist, PtrUnitFilter filter = IsEnemy) :Agent(u), DoSomeThingInRange(dist, filter), mdMoveWithDist(moveWithDist){}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Red);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		auto units = mUnit->getUnitsInRadius( mDist, mFilter);
		if (units.size())
		{
			// i must go away
			auto ep = units.getPosition();
			auto d = ep.getDistance(mUnit->getPosition());
			auto GoToPos = mUnit->getPosition() - ep;
			Vector2D vDir(GoToPos.x, GoToPos.y);
			vDir.Normalize();
			vDir = vDir * mdMoveWithDist;
			GoToPos.x = vDir.x;
			GoToPos.y = vDir.y;
			GoToPos = mUnit->getPosition() + GoToPos;
			mUnit->move(GoToPos);
			Broodwar->drawCircle(CoordinateType::Map, GoToPos.x, GoToPos.y, 25, Colors::Green);
			bool bResult = true;
		}

		return bResult;
	}

	void SetMoveWithDist(double dist) { mdMoveWithDist = dist; }
protected:
	double mdMoveWithDist;

};

inline Unit Closest(Position pos, Unitset set)
{
	Unit result = *set.begin();
	for (auto it : set)
	{
		if (it->getDistance(pos) < result->getDistance(pos))
			result = it;
	}

	return result;
}

class AgentAttackInRange : public Agent, public DoSomeThingInRange
{

public:
	AgentAttackInRange(Unit u, int dist, int switchRange, PtrUnitFilter filter = IsEnemy) :Agent(u), DoSomeThingInRange(dist, filter), mSwitchRange(switchRange){}
public:

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Orange);
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mSwitchRange, Colors::Purple);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		if(mUnit->isAttackFrame() || mUnit->isAttacking() || mUnit->isStartingAttack())
		{
			bResult = true;
		}
		else
		{
			auto inRange = mUnit->getUnitsInRadius(mDist, mFilter);
			auto swit = mUnit->getUnitsInRadius(mSwitchRange, mFilter);
			if (inRange.size() || swit.size())
			{
				Unit eu;
				if (swit.size())
					eu = LowHP(inRange);
				else if (inRange.size())
					eu = Closest(mUnit->getPosition(), inRange);

				if ( mUnit->getTarget() != eu || mUnit->getLastCommand().getType() != UnitCommandTypes::Attack_Unit)
					mUnit->attack(eu);
				bResult = true;
			}
			else if (mUnit->getLastCommand().getType() == UnitCommandTypes::Attack_Unit && mUnit->getTarget())
				bResult = true;
		}
		return bResult;
	}
protected:
	int mSwitchRange;
};

inline double CalcScore(Unitset set)
{
	double result = 0;
	for (auto it : set)
	{
		if (it->getType().isBuilding())
			continue;
		switch (it->getType())
		{
			// todo implemented
		default:
			result += it->getType().mineralPrice() + it->getType().gasPrice();
			break;
		}
	}
	return result;
}

class AssessTheEnemy : public Agent, public DoSomeThingInRange
{
public:
	AssessTheEnemy(Unit u, int dist, PtrUnitFilter filter = IsEnemy) :Agent(u), DoSomeThingInRange(dist, filter){}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Purple);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		double myArmyValue = CalcScore(mUnit->getUnitsInRadius(3*mDist, IsAlly));
		auto enemyArmy = mUnit->getUnitsInRadius(mDist, mFilter);
		double enemyArmyValue = CalcScore(enemyArmy);
		if (myArmyValue *1< enemyArmyValue)
		{
			auto ep = enemyArmy.getPosition();
			auto d = ep.getDistance(mUnit->getPosition());
			auto GoToPos = mUnit->getPosition() - ep;
			Vector2D vDir(GoToPos.x, GoToPos.y);
			vDir.Normalize();
			vDir = vDir * 30;
			GoToPos.x = vDir.x;
			GoToPos.y = vDir.y;
			GoToPos = mUnit->getPosition() + GoToPos;
			if(Broodwar->isWalkable(WalkPosition(GoToPos)))
				mUnit->move(GoToPos);
			Broodwar->drawCircle(CoordinateType::Map, GoToPos.x, GoToPos.y, 25, Colors::Green);
			bResult = true;
		}
		return bResult;
	}
};

class AgentStayToghether : public Agent, public DoSomeThingInRange
{
public:
	AgentStayToghether(Unit u, int dist, PtrUnitFilter filter = IsAlly) :Agent(u), DoSomeThingInRange(dist, filter){}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Grey);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		if (!mUnit->isMoving()) // try to stay together
		{
			auto friends = Broodwar->getUnitsInRadius(mUnit->getPosition(), mDist, mFilter);
			mUnit->move(friends.getPosition());

		}
		return bResult;
	}
};


class AgentGoToPosition : public Agent
{
public:
	AgentGoToPosition(Unit u, Position pos) :Agent(u), mPos(pos) {}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mPos.x, mPos.y, 13, Colors::Green);
	}


	virtual bool OnFrame() override
	{
		if(mUnit->isIdle())
			mUnit->move(mPos);
		return true;
	}

protected:
	Position mPos;
};


class ControlPattern
{
protected:
	std::vector<Agent*> Agents;
	Unit mUnit;
public:
	ControlPattern(Unit u) : mUnit(u){}
	virtual ~ControlPattern(){
		for (auto a : Agents) {
			delete a;
		}
	}
	void OnDraw()
	{
		for (auto it : Agents)
		{
			it->OnDraw();
		}

	}

	void OnFrame()
	{
		for (auto it : Agents)
		{
			if (it->OnFrame())
				break;
		}
	}

	bool isUnitDead()
	{
		return mUnit->getHitPoints() == 0;
	}
};
static
Position GetEnemyPos(Position &p)
{
	TilePosition pp(p);
	TilePosition result(Broodwar->getStartLocations().at(0));
	for (auto it : Broodwar->getStartLocations())
	{
		if (it.getDistance(pp) > result.getDistance(pp))
			result = it;
	}
	return Position(result);
}

class DragoonControl : public ControlPattern
{
public:
	DragoonControl(Unit drag) : ControlPattern(drag)
	{
		Agents.push_back(new AssessTheEnemy(drag, 300));
		Agents.push_back(new AgentStayAway(drag, 75, 100, IsEnemy));
		Agents.push_back(new AgentAttackInRange(drag, 250, 150));
		Agents.push_back(new AgentStayAway(drag, 10, 5, IsAlly));
		Agents.push_back(new AgentStayToghether(drag, 800));
		Agents.push_back(new AgentGoToPosition(drag, GetEnemyPos(drag->getPosition())));
	}
};

class ZelotControl : public ControlPattern
{

public:
	ZelotControl(Unit zelka) : ControlPattern(zelka)
	{
		Agents.push_back(new AssessTheEnemy(zelka, 500));
		Agents.push_back(new AgentAttackInRange(zelka, 100, 10));
		Agents.push_back(new AgentStayToghether(zelka, 800));
		Agents.push_back(new AgentGoToPosition(zelka, GetEnemyPos(zelka->getPosition())));
		//Agents.push_back(new AgentStayToghether(zelka, 500));
	}
};

struct UnitPatterPair
{
	BWAPI::UnitType type;
	ControlPattern * cp;
};

class Micro : public ManagerBase
{
	Micro() {}
	static Micro *inst;
public:
	static ManagerBase& GetInstance()
	{
		if (inst == NULL)
			inst = new Micro;
		return *inst;
	}


	virtual void ReleaseInst() override {}

	virtual void OnUnitComplete(Unit u) override
	{
		if (u->getType() == UnitTypes::Protoss_Zealot)
			mUnits.push_back(new ZelotControl(u));
		if (u->getType() == UnitTypes::Protoss_Dragoon)
			mUnits.push_back(new DragoonControl(u));
	}

	void OnFrame()
	{
		for (auto it = mUnits.begin(); it != mUnits.end();)
		{
			if ((*it)->isUnitDead())
				it = mUnits.erase(it);
			else
				it++;
		}
		for (auto it : mUnits)
		{
			it->OnFrame();
		//	it->OnDraw();
		}
	}
private:
	std::vector<ControlPattern *>  mUnits;

};


#endif