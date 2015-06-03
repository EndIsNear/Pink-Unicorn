#ifndef PINK_UNICORN_TASKS_QUEUE
#define PINK_UNICORN_TASKS_QUEUE

#include <vector>
#include <list>

typedef void (*TaskCallBack) ();
struct Task
{
	enum Type
	{
	};
	enum Priority
	{
		MAX = 0
	};
	Type mType;
	Priority mPriority;
	TaskCallBack mCallBack;
	unsigned TimeLimit; // in frames
	unsigned ID;
	bool isComplete;
};

class TaskQueue : public std::list<Task>
{
private:
	TaskQueue(){};
	TaskQueue& operator = (const TaskQueue &q){};
	static TaskQueue *inst;
public:

	static 
	TaskQueue& GetInstance()
	{
		if (!inst)
		{
			inst = new TaskQueue;
		}
		return *inst;
	}

	void ReleaseCompleteTasks();
	void GetTasksWithPriority(Task::Priority pri , std::list<Task*> &output);
	void GetTasksWithType(Task::Type type, std::list<Task*> &output);

	~TaskQueue()
	{
		delete inst;
	}
};

#endif