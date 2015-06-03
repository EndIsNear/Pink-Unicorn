#ifndef PINK_UNICORN_TASKS_QUEUE
#define PINK_UNICORN_TASKS_QUEUE

#include <vector>

typedef void (*TaskCallBack) ();
struct Task
{
	enum TaskType
	{
	};
	enum TaskPriority
	{
	};
	TaskType mType;
	TaskPriority mPriority;
	TaskCallBack mCallBack;
	unsigned TimeLimit;
};

class TaskQueue : public std::vector<Task*>
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

	~TaskQueue()
	{
		delete inst;
	}
};

#endif