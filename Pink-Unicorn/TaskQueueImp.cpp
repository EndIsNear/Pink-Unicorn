#include "TasksQueue.h"
#include <memory>

unsigned Task::nextID = 0;
TaskQueue* TaskQueue::inst = NULL;


void TaskQueue::ReleaseCompleteTasks()
{
	if (empty())
		return;
	for (auto it = begin(); it != end();)
	{
		if ((*it)->mIsComplete)
			it = erase(it);
		else
			it++;
	}
}

void TaskQueue::GetTasksWithPriority(Task::Priority pri, TaskList& output)
{
	for (auto it : *this)
		if (it->mPriority == pri)
			output.push_back(it);
}

void  TaskQueue::GetTasksWithType(Task::Type type, TaskList& output, bool bOnlyNotComplete /*true by default*/)
{
	for (auto it : *this)
	if (it->mType == type && it->mIsComplete == !bOnlyNotComplete)
			output.push_back(it);
}

