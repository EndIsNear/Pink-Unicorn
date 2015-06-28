#ifndef STRATEGY_MANAGER_H
#define STRATEGY_MANAGER_H

using namespace BWAPI;

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

	void UpdateSupply();
	void UpdateWorkers();
	void AddBase(Unit base)
	{
		m_bases.insert(base);
	}

private:
	Unitset m_bases;
};

#endif //STRATEGY_MANAGER_H