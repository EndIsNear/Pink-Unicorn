#include "ResourceManager.h"

using namespace BWAPI;



void ResourceManager::OnFrame()
{
	if (Broodwar->getFrameCount() - mLastUpdateFrame > 25) // every second
		UpdateState();
	else
	{
		TaskList tasks;
		TaskQueue::GetInstance().GetTasksWithType(Task::Resource, tasks);
		//tasks.sort(Task::CompTask);

		for (auto t : tasks)
		{
			if (ReleaseResourceTask *temp = dynamic_cast<ReleaseResourceTask*>(t.get()))
				ExecuteReleaseTask(*temp);
			else if (ReserveResourceTask*temp = dynamic_cast<ReserveResourceTask*>(t.get()))
				ExecuteReserveTask(*temp);
			else
				DEBUG_CHECK(false);
		}
	}
}

bool ResourceManager::Reserve(const ResourcePack &rRecPack)
{
		mReserved += rRecPack;
		return true;
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


void ResourceManager::ExecuteReserveTask(ReserveResourceTask &task)
{
	DEBUG_CHECK(task.mIsComplete == false);
	if (CanReserve(task.mResPack))
	{
		Reserve(task.mResPack);
		task.mIsComplete = true;
	}
}

void ResourceManager::ExecuteReleaseTask(ReleaseResourceTask &task)
{
	DEBUG_CHECK(task.mIsComplete == false);
	Release(task.mResPack);
	task.mIsComplete = true;
}


