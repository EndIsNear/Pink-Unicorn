#include "TasksQueue.h"


void TaskQueue::ReleaseCompleteTasks()
{
	for (auto it = begin(); it != end(); it++)
		if (it->isComplete)
			erase(it);
}

void TaskQueue::GetTasksWithPriority(Task::Priority pri , std::list<Task*> &output)
{
	for (auto it : *this)
		if (it.mPriority == pri)
			output.push_back(&it);
}

void  TaskQueue::GetTasksWithType(Task::Type type, std::list<Task*> &output)
{
	for (auto it : *this)
		if (it.mType == type)
			output.push_back(&it);
}

