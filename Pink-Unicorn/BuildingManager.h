#ifndef PINK_UNICORN_BUILDING_MANAGER_H
#define PINK_UNICORN_BUILDING_MANAGER_H

using namespace BWAPI;
typedef std::pair<UnitType, Unit> BuildingPair;


class BuildingManager : public ManagerBase
{
	//singleton
	BuildingManager(){};
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
private:
	std::vector <BuildingPair> m_BuildingsInProgress;
};


#endif