#ifndef PINK_UNICORN_BUILDING_MANAGER_H
#define PINK_UNICORN_BUILDING_MANAGER_H

const int SupplyPerPylon = 16;

using namespace BWAPI;
typedef std::pair<UnitType, Unit> BuildingPair;


class BuildingManager : public ManagerBase
{
	//singleton
	BuildingManager()
	{
		m_SupplyInProgress = 0;
	};
	BuildingManager(const BuildingManager&) = delete;
	BuildingManager& operator= (const BuildingManager&) = delete;
	static BuildingManager * m_instance;

public:
	static BuildingManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new BuildingManager;
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
		BuildingManager::_ReleaseInst();
	};

	virtual void OnFrame() override;


	bool Build(UnitType building);
	void CheckNewBuildings(Unit building);

	void OnUnitComplete(Unit unit)
	{
		if (unit->getType() == UnitTypes::Protoss_Pylon)
			m_SupplyInProgress -= SupplyPerPylon;
	}

	int GetSupplyInProgress()
	{
		return m_SupplyInProgress;
	}
private:
	std::vector <BuildingPair> m_BuildingsInProgress;
	int m_SupplyInProgress;
};


#endif