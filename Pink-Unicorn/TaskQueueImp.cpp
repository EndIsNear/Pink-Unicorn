#include "TasksQueue.h"
#include <memory>

unsigned Task::nextID = 0;
TaskQueue* TaskQueue::inst = NULL;


void TaskQueue::ReleaseCompleteTasks()
{
	for (auto it = begin(); it != end(); it++)
		if ((*it)->mIsComplete)
				erase(it);
}

void TaskQueue::GetTasksWithPriority(Task::Priority pri, TaskList& output)
{
	for (auto it : *this)
		if (it->mPriority == pri)
			output.push_back(it);
}

void  TaskQueue::GetTasksWithType(Task::Type type, TaskList& output)
{
	for (auto it : *this)
	if (it->mType == type)
			output.push_back(it);
}

