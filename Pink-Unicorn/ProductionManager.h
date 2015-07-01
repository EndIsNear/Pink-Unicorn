#ifndef PINK_UNICORN_PRODUCE_MANAGER_H
#define PINK_UNICORN_PRODUCE_MANAGER_H

using namespace BWAPI;

class ProduceManager : public ManagerBase
{
	//singleton
	ProduceManager(){};
	ProduceManager(const ProduceManager&) = delete;
	ProduceManager& operator= (const ProduceManager&) = delete;
	static ProduceManager * m_instance;

public:
	static ProduceManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ProduceManager;
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
		ProduceManager::_ReleaseInst();
	};

	virtual void OnFrame() override;
	virtual void OnUnitComplete(BWAPI::Unit unit) override;

	bool ProduceSingleUnit(UnitType unit);
	bool ProduceSingleUnitFrom(UnitType unit, Unit producer);
};

#endif