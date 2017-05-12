#ifndef SMS_SCHEDULER_HPP
#define SMS_SCHEDULER_HPP

#include "Thread.hpp"

namespace sms
{

struct Scheduler
{
    Scheduler(Thread* threadContext);
    virtual ~Scheduler();
    virtual void setThread(Thread* thread);
    virtual bool hasScheduledTasks() {return hasPendingTasks();}
    virtual bool hasPendingTasks() = 0;
    virtual void execPendingTasks() = 0;
protected:
    Thread* _thread;
private:
    Thread _threadDefaultContext;
};

}

#endif
