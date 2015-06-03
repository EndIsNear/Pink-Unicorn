#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "TasksQueue.h"
#include <assert.h>

struct ResourcePack
{
	int minerals;
	int gas;
	int supply; // this will represent Total - Used supply

	ResourcePack() : minerals(0) , gas(0), supply(0){}

	bool IsCorrectState()
	{
		DEBUG_CHECK(minerals >= 0 && gas >= 0 && supply >= 0);
		if (minerals >= 0 && gas >= 0 && supply >= 0)
			return true;
		return false;
	}

	ResourcePack& operator+=(const ResourcePack &rh)
	{
		DEBUG_EXP(IsCorrectState());

		minerals += rh.minerals;
		gas += rh.gas;
		supply += rh.supply;

		DEBUG_EXP(IsCorrectState());
		return *this;
	}

	ResourcePack& operator-=(const ResourcePack &rh)
	{
		DEBUG_EXP(IsCorrectState());

		minerals -= rh.minerals;
		gas -= rh.gas;
		supply -= rh.supply;

		DEBUG_EXP(IsCorrectState());
		return *this;
	}

	ResourcePack& operator/=(int frames)
	{
		DEBUG_EXP(IsCorrectState());

		minerals /= frames;
		gas /= frames;
		supply /= frames;

		DEBUG_EXP(IsCorrectState());
		return *this;
	}

};


class ResourceManager : public ManagerBase
{

	public:
		virtual void OnFrame() override;
		virtual void CheckForNewTask() override {};

	public:

		ResourceManager() { UpdateState() }
		~ResourceManager() {}


		const ResourcePack& GetCurrentResourceState() const { return mRatePerMin; }
		const ResourcePack& GetResourceRate() const { return mCurrent; }
		const ResourcePack& GetReserved() const { return mReserved; }

		bool  Reserve(const ResourcePack &rRecPack);
		bool	CanReserve(const ResourcePack &rRecPack);


		//this will get actual information from bwapi and recalc ResourceRate
		void UpdateState();

		void ReleaseReserve(const ResourcePack & rRecPack);
	private:
		void UpdateCurrentResource();
		void UpdateResourceRate(const ResourcePack & rPrevPack);

		// only for debug 
		void CheckState(); 

	private:
		ResourceManager& operator=(const ResourceManager &o){ DEBUG_CHECK(false); /*you try to do bad things */ }
	private:
		int mLastUpdateFrame;
		ResourcePack mCurrent, mReserved, mRatePerMin;

};


#endif

