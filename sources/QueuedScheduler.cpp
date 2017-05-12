#include "sms/QueuedScheduler.hpp"

#include <cassert>
#include <iostream>

namespace sms
{

QueuedScheduler::QueuedScheduler(Thread* threadContext)
    : Scheduler(threadContext), _idle(true)
{
}

void QueuedScheduler::execTask(std::unique_ptr<Task> && task)
{
    {
        std::lock_guard<std::mutex> guard(_taskListMutex);
        _tasks.emplace_back(std::move(task));
    }

    //if is is single task and we are in proper thread context, then execute it immediately
    if (_thread->checkThreadContext() && _idle && _tasks.size() == 1)
    {
        // Execute scheduled task immediately.
        execPendingTasks();
    }
    else
		_thread->notifyTaskAdded();// Postpone scheduled task as another task is executed or it is added from another thread."
}

bool QueuedScheduler::hasScheduledTasks()
{
    return !_idle || hasPendingTasks();
}

bool QueuedScheduler::hasPendingTasks()
{
    std::lock_guard<std::mutex> guard(_taskListMutex);
    return !_tasks.empty();
}

void QueuedScheduler::execPendingTasks()
{
    if (!_thread->checkThreadContext())
        assert(_thread->checkThreadContext() && "Call of execPendingTasks() from another thread context prohibited");

    if (!_idle)
        return;

    _idle = false;

    bool taskExists;
    {
        std::lock_guard<std::mutex> guard(_taskListMutex);
        taskExists = !_tasks.empty();
    }

    while (taskExists)
    {
        TaskPtr task;
        {
            std::lock_guard<std::mutex> guard(_taskListMutex);
            //extract first element of the scheduler task list
            task = std::move(*_tasks.begin());
            _tasks.pop_front();
        }

        //execute extracted task
        task->exec();

        //check for more tasks
        {
            std::lock_guard<std::mutex> guard(_taskListMutex);
            taskExists = !_tasks.empty();
        }
    }

    _idle = true;
}

void QueuedScheduler::idle()
{
    assert(_thread->checkThreadContext() && "Call of idle() from another thread context prohibited");

    _thread->idle(this);
}

}
