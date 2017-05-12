#include "sms/Thread.hpp"

#include "sms/QueuedScheduler.hpp"

#include <string>
#include <sstream>
#include <thread>

namespace sms
{

RealtimeClock Thread::_defaultClock;

Thread::Thread(Clock* clock)
    : _thisThreadId(std::this_thread::get_id())//TODO: create thread ID after thread started
    , _clock(clock)
{
}

void Thread::start()
{
    if (!_threadRunning)
    {
        _threadRunning = true;
        _thread = std::thread(&Thread::threadLoop, this);
    }
}

void Thread::terminate()
{
    if (_threadRunning)
    {
        _threadRunning = false;
        notifyTaskAdded();
        _thread.join();
        //move therd to terminating thread context
        _thisThreadId = std::this_thread::get_id();
    }
}

bool Thread::checkThreadContext()
{
    bool inThisThread = (_thisThreadId == std::this_thread::get_id());
    return inThisThread;
}

void Thread::addScheduler(Scheduler * scheduler)
{
    _schedulers.push_back(scheduler);
}

void Thread::removeScheduler(Scheduler * scheduler)
{
    std::list<Scheduler*>::iterator it = _schedulers.begin();
    while (it != _schedulers.end())
    {
        if (*it == scheduler)
        {
            _schedulers.erase(it);
            break;
        }
        it++;
    }
}

void Thread::threadLoop()
{
    _thisThreadId = std::this_thread::get_id();
    std::unique_lock<std::mutex> lock(_doTasksMutex);
    while (_threadRunning)
    {
        doTasks();
        while (!notifiedTaskAdded)
            _taskAddedCondition.wait(lock);
    }
}

void Thread::doTasks()
{
    //process all pending tasks
    for (Scheduler* scheduler : _schedulers)
        scheduler->execPendingTasks();
}

void Thread::notifyTaskAdded()
{
    //std::unique_lock<std::mutex> lock(_doTasksMutex);
    notifiedTaskAdded = true;
    _taskAddedCondition.notify_all();
}

void Thread::idle(Scheduler* idleScheduler)
{
    for (Scheduler* scheduler : _schedulers)
    {
        if (scheduler != idleScheduler)
            scheduler->execPendingTasks();
        else
            if (scheduler->hasPendingTasks())
                return;
            else
                ;//std::this_thread::sleep_for(std::chrono::milliseconds(100));//TODO: sleep for exact time until next sync event
    }
}

Clock::TimePoint Thread::getTime()
{
    Clock::TimePoint timeElapsed = _clock->now();
    return timeElapsed;
}

std::mutex& Thread::getMutex()
{
    return _mutex;
}

}
