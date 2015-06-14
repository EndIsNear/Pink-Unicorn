#ifndef PINK_UNICORN_AGENT_H
#define PINK_UNICORN_AGENT_H


#include <BWAPI.h>
#include <BWAPI/Client.h>
#include "ManagerBase.h"

using namespace BWAPI;
using namespace Filter;


class Agent
{
public:
	Agent(Unit u) : mUnit(u) { }
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
	AgentStayAway(Unit u, int dist, PtrUnitFilter filter = IsEnemy) :Agent(u), DoSomeThingInRange(dist, filter){}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Red);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		auto enemy = Broodwar->getUnitsInRadius(mUnit->getPosition(), mDist, mFilter);
		if (enemy.size())
		{
			// i must go away
			auto ep = enemy.getPosition();
			auto d = ep.getDistance(mUnit->getPosition());
			auto GoToPos = mUnit->getPosition() - ep;
			GoToPos = GoToPos * 3;
			GoToPos = mUnit->getPosition() + GoToPos;
			mUnit->move(GoToPos);
			Broodwar->drawCircle(CoordinateType::Map, GoToPos.x, GoToPos.y, 25, Colors::Green);
			bool bResult = true;
		}

		return bResult;
	}


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
		if (mUnit->getGroundWeaponCooldown() == 0)
		{
			auto inRange = mUnit->getUnitsInRadius(mDist, mFilter);
			auto swit = mUnit->getUnitsInRadius(mSwitchRange, mFilter);
			if (swit.size())
			{
				mUnit->attack(LowHP(inRange));
				bResult = true;
			}
			else if (inRange.size())
			{
				mUnit->attack(Closest(mUnit->getPosition(), inRange));
				bResult = true;
			}
		}
		return bResult;
	}
protected:
	int mSwitchRange;
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
		if (!mUnit->isMoving() && mUnit->getGroundWeaponCooldown() == 0) // try to stay together
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
		if (mUnit->isIdle())
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

class DragoonControl : public ControlPattern
{
public:
	DragoonControl(Unit drag) : ControlPattern(drag)
	{
		Agents.push_back(new AgentStayAway(drag, 50));
		Agents.push_back(new AgentAttackInRange(drag, 250, 150));
		Agents.push_back(new AgentStayToghether(drag, 200));
		Position p(Broodwar->getStartLocations().at(1));
		Agents.push_back(new AgentGoToPosition(drag, p));
	}
};

class ZelotControl : public ControlPattern
{

public:
	ZelotControl(Unit zelka) : ControlPattern(zelka)
	{
		Agents.push_back(new AgentAttackInRange(zelka, 250, 25));
		Position p(Broodwar->getStartLocations().at(1));
		Agents.push_back(new AgentGoToPosition(zelka, p));
		//Agents.push_back(new AgentStayToghether(zelka, 50));
	}
};


class Micro : public ManagerBase
{
public:
	Micro(Unitset Army)
	{
		mRadius = 800;
		mBattleField = Army.getPosition();
		for (auto it : Army)
		{
			if (it->getType() == UnitTypes::Protoss_Zealot)
				mUnits.push_back(new ZelotControl(it));
			if (it->getType() == UnitTypes::Protoss_Dragoon)
				mUnits.push_back(new DragoonControl(it));

		}
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
			it->OnDraw();
		}
	}
private:
	std::vector<ControlPattern *>  mUnits;
	Position mBattleField;
	int mRadius;

};
#endif