#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H


#include "ManagerBase.h"
#include "TasksQueue.h"

struct ResourcePack
{
	int minerals;
	int gas;
	int supply;
};


class ResourceManager : public ManagerBase
{

	public:
		virtual void OnFrame() override {};
		virtual void CheckForNewTask() override {};

	public:
		const ResourcePack& GetCurrentResourceState() const { return mRatePerMin; }
		const ResourcePack& GetResourceRatr() const { return mCurrent; }
		const ResourcePack& GetReserved() const { return mReserved; }

		bool  Reserve(const ResourcePack &);


	private:


		ResourcePack mCurrent, mReserved, mRatePerMin;

};


#endif

