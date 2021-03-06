#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

using namespace BWAPI;

class WorkerManager : public ManagerBase
{
	//singleton
	WorkerManager()
	{
		m_maxMin = 3;
		m_maxGas = 3;
		m_workersCnt = 4;
		m_maxWorkers = 70;
	};
	WorkerManager(const WorkerManager&) = delete;
	WorkerManager& operator= (const WorkerManager&) = delete;
	static WorkerManager * m_instance;

public:
	static WorkerManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new WorkerManager;
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
		WorkerManager::_ReleaseInst();
	};


	virtual void OnStart() override;
	virtual void OnFrame() override;

	int GetNeededWorkers();
	int GetNeededWorkersForBase(Unit base);

	//adding worker, base or gas station
	//first unit have to be base
	void AddUnit(Unit);

	//release a worker near to the wanted position
	bool ReleaseWorker(Position pos, Unit& result);

	void SetWorkPerMin(size_t workers)
	{
		m_maxMin = workers;
	}

	void SetWorkPerGas(size_t workers)
	{
		m_maxGas = workers;
	}

	void SetMaxWorkersCnt(size_t workers)
	{
		m_maxWorkers = workers;
	}

private:
	struct Expand
	{
		Unit base;
		//workers for this base
		Unitset workers;

		//res neat to the base
		Unitset gasStations;//stations and geysers
		Unitset minerals;
	};

	//max workers per mineral depo
	size_t m_maxMin;
	//max workers per gas station
	size_t m_maxGas;
	size_t m_workersCnt;
	size_t m_maxWorkers;
	std::vector<Expand> m_expands;

	void RemoveDeadWorkers(size_t idx);
	void BalanceWorkerBetweenExpands();
	void moveWorkersFromTo(size_t cnt, size_t idxFrom, size_t idxTo);
};

#endif //WORKER_MANAGER_H
