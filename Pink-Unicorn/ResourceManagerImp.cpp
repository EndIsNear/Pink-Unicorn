#include "ResourceManager.h"

using namespace BWAPI;

ResourceManager * ResourceManager::m_instance = NULL;

void ResourceManager::OnFrame()
{

}

bool ResourceManager::Reserve(const ResourcePack &rRecPack)
{
		mReserved += rRecPack;
		return true;
}


bool ResourceManager::CanReserve(const ResourcePack &rRecPack)
{
	if (mReserved.minerals + rRecPack.minerals <= mCurrent.minerals &&
			mReserved.gas + rRecPack.gas <= mCurrent.gas &&
			mReserved.supply + rRecPack.supply <= mCurrent.supply)
		return true;
	return false;
}


void ResourceManager::UpdateState()
{
	ResourcePack prevState = mCurrent;
	UpdateCurrentResource();
	UpdateResourceRate(prevState);
}

void ResourceManager::UpdateCurrentResource()
{
	DEBUG_EXP(mCurrent.IsCorrectState());
	Player my = Broodwar->self();
	mCurrent.minerals = my->minerals();
	mCurrent.gas = my->gas();
	mCurrent.supply = my->supplyTotal() - my->supplyUsed();

	DEBUG_EXP(mCurrent.IsCorrectState());
}

void ResourceManager::UpdateResourceRate(const ResourcePack & rPrevPack)
{
	DEBUG_EXP(CheckState());


	int currenFrame = Broodwar->getFrameCount();
	/* to do implement 
	mRatePerMin = mCurrent;
	mRatePerMin -= rPrevPack;

	mRatePerMin /= currenFrame - mLastUpdateFrame;
	*/
	mLastUpdateFrame = currenFrame;


	DEBUG_EXP(CheckState());
}


void ResourceManager::CheckState()
{
	mCurrent.IsCorrectState();
	mRatePerMin.IsCorrectState();
	mReserved.IsCorrectState();
}


void ResourceManager::Release(const ResourcePack & rRecPack)
{
	mReserved -= rRecPack;
	UpdateCurrentResource();
}


bool ResourceManager::ExecuteReserveTask(unsigned min, unsigned gas, unsigned sup)
{
	ResourcePack tmp(min, gas, sup);
	if (CanReserve(tmp))
	{
		Reserve(tmp);
		return true;
	}
	return false;
}

void ResourceManager::ExecuteReleaseTask(unsigned min, unsigned gas, unsigned sup)
{
	ResourcePack tmp(min, gas, sup);
	Release(tmp);
}
