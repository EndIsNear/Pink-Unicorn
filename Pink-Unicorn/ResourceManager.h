#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include "DebugStuff.h"
#include "ManagerBase.h"
#include "ResourcePack.h"

class ResourceManager : public ManagerBase
{
	//singleton
	ResourceManager() :
	mCurrent(ResourcePack::ZeroTag()),
	mReserved(ResourcePack::ZeroTag()),
	mRatePerMin(ResourcePack::ZeroTag())
	{
		UpdateState();
	}
	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator= (const ResourceManager&) = delete;
	static ResourceManager * m_instance;

public:
	static ResourceManager& GetInstance()
	{
		if (!m_instance)
		{
			m_instance = new ResourceManager;
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
		ResourceManager::_ReleaseInst();
	};


	virtual void OnFrame() override;

	bool ReserveRes(unsigned, unsigned, unsigned);
	void ReleaseRes(unsigned, unsigned, unsigned);
protected:
	const ResourcePack& GetCurrentResourceState() const { return mRatePerMin; }
	const ResourcePack& GetResourceRate() const { return mCurrent; }
	const ResourcePack& GetReserved() const { return mReserved; }

	unsigned GetFreeMineral(){ return mCurrent.minerals - mReserved.minerals; }
	unsigned GetFreeGas(){ return mCurrent.gas - mReserved.gas; }
	unsigned GetFreeSupply(){ return  mCurrent.supply- mReserved.supply; }

	bool Reserve(const ResourcePack &rRecPack);
	bool CanReserve(const ResourcePack &rRecPack);

	//this will get actual information from bwapi and recalc ResourceRate
	void UpdateState();
	void Release(const ResourcePack & rRecPack);
private:
	void UpdateCurrentResource();
	void UpdateResourceRate(const ResourcePack & rPrevPack);

	// only for debug 
	void CheckState(); 
	int mLastUpdateFrame;
	ResourcePack mCurrent, mReserved, mRatePerMin;
};




#endif

