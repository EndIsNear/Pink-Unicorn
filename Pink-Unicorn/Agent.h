#ifndef PINK_UNICORN_AGENT_H
#define PINK_UNICORN_AGENT_H


#include <BWAPI.h>
#include <BWAPI/Client.h>
#include <algorithm>
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

inline Unit LowHP(Unitset &us)
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
	DoSomeThingInRange(int dist, const UnitFilter & filter) :mDist(dist), mFilter(filter) {}
protected:
	UnitFilter  mFilter;
	int mDist;
};


class AgentStayAway : public Agent, public DoSomeThingInRange
{
public:
	AgentStayAway(Unit u, int dist, double moveWithDist, const UnitFilter &filter) :Agent(u), DoSomeThingInRange(dist, filter), mdMoveWithDist(moveWithDist){}

	virtual void OnDraw() override
	{
		Broodwar->drawCircle(CoordinateType::Map, mUnit->getPosition().x, mUnit->getPosition().y, mDist, Colors::Red);
	}

	virtual bool OnFrame() override
	{
		bool bResult = false;
		auto units = mUnit->getUnitsInRadius( mDist, mFilter);
		if (LastTarget.isValid() && mUnit->getPosition().getDistance(LastTarget) > 10)
		{
			bResult = true;
			LastTarget = Positions::Invalid;
		}
		else
		{
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
				LastTarget = GoToPos;
				Broodwar->drawCircle(CoordinateType::Map, GoToPos.x, GoToPos.y, 25, Colors::Green);
				bool bResult = true;
			}
		}

		return bResult;
	}

	void SetMoveWithDist(double dist) { mdMoveWithDist = dist; }
protected:
	Position LastTarget;
	double mdMoveWithDist;

};

inline Unit Closest(Position &pos, Unitset &set)
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
	AgentAttackInRange(Unit u, int dist, int switchRange, const UnitFilter &filter) :Agent(u), DoSomeThingInRange(dist, filter), mSwitchRange(switchRange){}
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

				if (mUnit->getTarget() != eu || mUnit->getLastCommand().getType() != UnitCommandTypes::Attack_Unit)
					mUnit->attack(eu->getPosition());
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

inline double CalcScore(Unitset &set)
{
	double result = 0;
	for (auto it : set)
	{
		if (IsBuilding(it) || IsWorker(it))
			continue;
		switch (it->getType())
		{

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
	AssessTheEnemy(Unit u, int dist, const UnitFilter &filter) :Agent(u), DoSomeThingInRange(dist, filter){}

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
			vDir = vDir * 100;
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
	AgentStayToghether(Unit u, int dist, const UnitFilter &filter) :Agent(u), DoSomeThingInRange(dist, filter){}

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
			if (friends.size() < mDist *0.05)
			{
				mUnit->move(friends.getPosition());
				bResult = true;
			}
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
	void PushAgent(Agent* a) { Agents.push_back(a); };
	virtual ~ControlPattern()
	{
	//	for (auto a : Agents) 
	//	{
	//		delete a;
	//	}
	}

	Unit GetUnit() {
		return mUnit;
	}
	void OnDraw()
	{
		for (auto it : Agents)
		{
			it->OnDraw();
		}

	}

	virtual void OnFrame()
	{
		if (mUnit->isStuck())
		{
			mUnit->move(Position(Broodwar->self()->getStartLocation()));
		}
		for (auto it : Agents)
		{
			if (it->OnFrame())
				break;
		}
	}

	bool isUnitDead()
	{
		return  !mUnit->exists();
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
	DragoonControl(Unit drag);
};

class ZelotControl : public ControlPattern
{

public:
	ZelotControl(Unit zelka);
};


class MicroControler
{
public:
	MicroControler() : next(0)
	{

	}

	void Clear()
	{
	//	for (auto it : mUnits)
		//	delete it;

	}

	~MicroControler()
	{
		Clear();
	}

	unsigned GetSize() { return mUnits.size(); }

	void SwitchTargetPoint(Position pos)
	{
		Unitset us;
		for (auto it : mUnits)
			us.insert(it->GetUnit());
		Clear();
		for (auto it : us)
			AddUnit(it, pos);
	}

	void AddUnit(Unit u, Position TargetPos) 
	{
		ControlPattern *pat = new ControlPattern(u);
		if (u->getType() == UnitTypes::Protoss_Zealot)
		{
			
			pat->PushAgent(new AgentStayAway(u, 75, 50, IsEnemy && !IsBuilding));
			pat->PushAgent(new AgentAttackInRange(u, 250, 150, IsEnemy && !IsBuilding));
			pat->PushAgent(new AgentGoToPosition(u, TargetPos));

		}
		else if (u->getType() == UnitTypes::Protoss_Dragoon)
		{
		//	pat->PushAgent(new AgentStayAway(u, 75, 50, IsEnemy && !IsBuilding));
			pat->PushAgent(new AgentAttackInRange(u, 250, 150, IsEnemy && !IsBuilding));
			pat->PushAgent(new AgentGoToPosition(u, TargetPos));
		}
		else if (u->getType() == UnitTypes::Protoss_Observer)
		{
			pat->PushAgent(new AgentStayToghether(u, 150, IsAlly));
			pat->PushAgent(new AgentGoToPosition(u, TargetPos));
		}
		mUnits.push_back(pat);
	}

	/*this is extremly slow*/
	void  GetUnits(Unitset &output)
	{
		for (auto it : mUnits)
			output.insert(it->GetUnit());
	}

	void GetUnitsWithType(Unitset &output, UnitType type)
	{
		for (auto it : mUnits)
			if (it->GetUnit()->getType() == type)
				output.insert(it->GetUnit());

	}

	void OnFrame(unsigned  Time)
	{

		for (auto it = mUnits.begin(); it != mUnits.end();)
		{
			if ((*it)->isUnitDead())
			{
//delete *it;
				it = mUnits.erase(it);
			}
			else
				it++;
		}
		
		if (mUnits.size())
		{
			for (unsigned i = 0; i < Time; i++)
			{
			if (next >=  mUnits.size())
					next = 0;

				mUnits[next]->OnFrame();
				next++;
			}
		}
	}
private:
	unsigned next;
	std::vector<ControlPattern *>  mUnits;

};


#endif