#ifndef STRATEGY_MANAGER_H
#define STRATEGY_MANAGER_H

#include <queue>

using namespace BWAPI;

typedef std::pair<int, std::function<bool()>> Task;

class StrategyManager : public ManagerBase
{
	//singleton
	StrategyManager(){};
	StrategyManager(const StrategyManager&) = delete;
	StrategyManager& operator= (const StrategyManager&) = delete;
	static StrategyManager * m_instance;

public:
	static StrategyManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new StrategyManager;
		}
		return *m_instance;
	}

	static void _ReleaseInst()
	{
		if (m_instance)
		{
			delete m_instance;
			m_instance = NULL;
		}
	}

	virtual void ReleaseInst()
	{
		StrategyManager::_ReleaseInst();
	};

	virtual void OnStart() override;
	virtual void OnFrame() override;

	void AddBase(Unit base)
	{
		m_bases.insert(base);
	}
	bool BuildExpand();
	Position::list GetExpandPos();
private:
	void UpdateSupply();
	void UpdateWorkers();

	//task things :)
	class cmpTasks
	{
		bool reverse;
	public:
		cmpTasks(const bool revParam = false)
		{
			reverse = revParam;
		}

		bool operator() (const Task& left, const Task& right) const
		{
			if (reverse) return (left.first > right.first);
			else return (left.first < right.first);
		}
	};

	std::priority_queue<Task, std::vector<Task>, cmpTasks> m_TaskQ;

	void PushBuildTask(int priority, UnitType type, TilePosition nearTo = TilePositions::None);
	void PushProductionTask(int priority, UnitType type/*, TilePosition nearTo*/);
	void PushExpandTask(int priority);

	void CheckQueue();

	Unitset m_bases;
};

#endif //STRATEGY_MANAGER_H