#ifndef SMS_QUEUEDSCHEDULER_HPP
#define SMS_QUEUEDSCHEDULER_HPP

#include "Scheduler.hpp"
#include "Task.hpp"
#include "Thread.hpp"
#include <list>
#include <memory>

namespace sms {

struct Thread;

struct QueuedScheduler : public Scheduler
{
    QueuedScheduler(Thread* threadContext);
    void execTask(std::unique_ptr<Task> && task);
    bool hasScheduledTasks() override;
    bool hasPendingTasks() override;
    void execPendingTasks() override;
    void idle();
    ~QueuedScheduler() {}
private:
    typedef std::unique_ptr<Task> TaskPtr;
    typedef std::list<TaskPtr> TaskList;
    TaskList _tasks;
    //mutex for multithreaded access to _tasks
    std::mutex _taskListMutex;
    //tasks execution status
    bool _idle;
};

}

#endif
