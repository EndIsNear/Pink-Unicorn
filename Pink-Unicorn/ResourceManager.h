#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "TasksQueue.h"
#include <assert.h>
#include "ResourceTasks.h"

struct ResourcePack
{
	int minerals;
	int gas;
	int supply; // this will represent Total - Used supply

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


	static ResourcePack NeedFor(BWAPI::UnitType &type)
	{
		return ResourcePack{ type.mineralPrice(), type.gasPrice(), type.supplyRequired() };
	}
};

class ResourceManager : public ManagerBase
{

	public:
		virtual void OnFrame() override;
		virtual void CheckForNewTask() override {};

	

		ResourceManager() { UpdateState(); }
		~ResourceManager() {}

	protected:

//		void ExecuteReserveTask(ReserveResourceTask &task);
//		void ExecuteReleaseTask(ReleaseResourceTask &task);


		const ResourcePack& GetCurrentResourceState() const { return mRatePerMin; }
		const ResourcePack& GetResourceRate() const { return mCurrent; }
		const ResourcePack& GetReserved() const { return mReserved; }

		unsigned GetFreeMineral(){ return mCurrent.minerals - mReserved.minerals; }
		unsigned GetFreeGas(){ return mCurrent.gas - mReserved.gas; }
		unsigned GetFreeSupply(){ return  mCurrent.supply- mReserved.supply; }


		bool  Reserve(const ResourcePack &rRecPack);
		bool	CanReserve(const ResourcePack &rRecPack);


		//this will get actual information from bwapi and recalc ResourceRate
		void UpdateState();

		void Release(const ResourcePack & rRecPack);
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

