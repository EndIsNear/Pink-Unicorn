#ifndef PINK_UNICORN_TASKS_QUEUE
#define PINK_UNICORN_TASKS_QUEUE

#include <vector>
#include <memory>
#include <list>

class Task;

typedef std::shared_ptr<Task> TaskPtr;
typedef std::list<TaskPtr> TaskList;



typedef void(*TaskCallBack) (TaskPtr);
class Task
{
public:
	enum Type
	{
		None,
		Produce,
		Resource,
		TakeControl,
		Explore
	};
	enum Priority
	{
		MAX = 0,
		HIGH = 1,
		MID = 2,
		LOW = 3,
		Zero = UINT_MAX
	};

protected:
	Task() :
		mID(GetNextID()),
		mType(None),
		mPriority(Zero),
		mCallBack(NULL),
		mIsComplete(false),
		mIsDenied(false){}
	static 
		unsigned GetNextID() { return nextID++; }
public:
	const unsigned mID;

	Type mType;
	Priority mPriority;
	TaskCallBack mCallBack;
//	unsigned mTimeLimit; // in frames
	TaskList mSubTasks;
	bool mIsComplete;
	bool mIsDenied;
public:
	virtual ~Task(){};

	static bool CompTask(const TaskPtr& first, const TaskPtr& second)
	{
		return first->mPriority < second->mPriority;
	}

	static bool AllSubTasksAreDone(TaskList &l)
	{
		for (auto it : l)
		{
			if (it->mIsComplete == false)
				return false;
		}
		return true;
	}
private:
	static unsigned nextID;
};


class TaskQueue : public TaskList
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
	void GetTasksWithPriority(Task::Priority pri, TaskList& output);
	void GetTasksWithType(Task::Type type, TaskList& output, bool bOnlyNotComplete = true);

	~TaskQueue()
	{
		delete inst;
	}
};

#endif
