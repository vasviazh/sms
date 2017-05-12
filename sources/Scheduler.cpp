#include "sms/Scheduler.hpp"
#include "sms/Thread.hpp"

namespace sms
{

Scheduler::Scheduler(Thread* threadContext)
    : _thread(threadContext)
{
    _thread->addScheduler(this);
}

Scheduler::~Scheduler()
{
    _thread->removeScheduler(this);
}

void Scheduler::setThread(Thread* thread)
{
    //remove from old thread context
    _thread->removeScheduler(this);

    _thread = thread;
    _thread->addScheduler(this);
}

}
