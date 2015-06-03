#include "ResourceManager.h"

using namespace BWAPI;


bool ResourceManager::Reserve(const ResourcePack &rRecPack)
{
	if (CanReserve(rRecPack))
	{
		mReserved += rRecPack;
		return true;
	}
	return false;
}


bool	ResourceManager::CanReserve(const ResourcePack &rRecPack)
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

	ResourcePack mRatePerMin = mCurrent;
	mRatePerMin -= rPrevPack;

	mRatePerMin /= currenFrame - mLastUpdateFrame;
	mLastUpdateFrame = currenFrame;


	DEBUG_EXP(CheckState());
}


void ResourceManager::CheckState()
{
	mCurrent.IsCorrectState();
	mRatePerMin.IsCorrectState();
	mReserved.IsCorrectState();
}


void ResourceManager::ReleaseReserve(const ResourcePack & rRecPack)
{
	mReserved -= rRecPack;
}


