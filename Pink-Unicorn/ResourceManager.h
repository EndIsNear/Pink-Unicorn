#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"



class ResourceManager : public ManagerBase
{

	public:
		virtual void OnFrame() override;
		virtual void CheckForNewTask() override {};

	

		ResourceManager()
			:
			mCurrent(ResourcePack::ZeroTag()),
			mReserved(ResourcePack::ZeroTag()),
			mRatePerMin(ResourcePack::ZeroTag())
		{ UpdateState(); }
		~ResourceManager() {}

	protected:

		void ExecuteReserveTask(ReserveResourceTask &task);
		void ExecuteReleaseTask(ReleaseResourceTask &task);


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

