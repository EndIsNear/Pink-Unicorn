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


	bool Build(UnitType building);//near to start location
	bool BuildNearTo(UnitType building, TilePosition pos);
	bool BuildBaseExit(UnitType building);

	void CheckNewBuildings(Unit building);

	void OnUnitComplete(Unit unit);

	int GetSupplyInProgress()
	{
		return m_SupplyInProgress;
	}

	void AddSupplyInProgress()
	{
		m_SupplyInProgress += SupplyPerPylon;
	}
private:
	int m_SupplyInProgress;

	//use it after reserve res 
	bool BuildWithNearTo(Unit builder, UnitType building, TilePosition pos);
};


#endif