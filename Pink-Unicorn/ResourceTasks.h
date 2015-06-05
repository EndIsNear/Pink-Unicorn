#ifndef PINK_UNICORN_RESOURCE_TASKS_H
#define PINK_UNICORN_RESOURCE_TASKS_H

#include "ResourcePack.h"
#include "TasksQueue.h"

struct ResourcePack;

class ReserveResourceTask : public Task
{
	public:
		ReserveResourceTask(ResourcePack &rPack, Task::Priority pri)
		{
			mResPack = rPack;
			mPriority = pri;
			mType = Task::Resource;
		}
		ResourcePack mResPack;
};

class ReleaseResourceTask : public Task
{
public:
	ReleaseResourceTask(ResourcePack &rPack) 
	{
		mResPack = rPack;
		mPriority = Task::MAX;
		mType = Task::Resource;
	}

public:
	ResourcePack mResPack;
};

#endif