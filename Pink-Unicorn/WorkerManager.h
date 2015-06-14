#ifndef WORKER_MANAGER_H
#define WORKER_MANAGER_H

using namespace BWAPI;

class WorkerManager : public ManagerBase
{
public:
	WorkerManager(){}
	~WorkerManager(){}

	virtual void OnStart() override;
	virtual void OnFrame() override;


	//adding worker, base or gas station
	//first unit have to be base
	void AddUnit(Unit);

	//release a worker near to the wanted position
	Unit ReleaseWorker(Position pos);

	void SetWorkPerMin(size_t workers)
	{
		m_mineralWorkers = workers;
	}
	void SetWorkPerGas(size_t workers)
	{
		m_gasWorkers = workers;
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

	size_t m_workers;
	size_t m_mineralWorkers;
	size_t m_gasWorkers;
	//max workers per mineral depo
	size_t m_maxMin;
	//max workers per gas station
	size_t m_maxGas;
	std::vector<Expand> m_expands;
};

#endif //WORKER_MANAGER_H