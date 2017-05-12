#ifndef SMS_THREAD_HPP
#define SMS_THREAD_HPP

#include "Timer.hpp"

#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <condition_variable>

namespace sms {

struct Scheduler;

struct Thread
{
    Thread(Clock* clock = &_defaultClock);
    bool checkThreadContext();
    void doTasks();
    void idle(Scheduler* scheduler);
    void notifyTaskAdded();
    void start();
    void terminate();
    std::mutex& getMutex();
   //AsyncScheduler* getScheduler() {return &_asyncScheduler;}
    Clock::TimePoint getTime();
private:
    void threadLoop();
    void addScheduler(Scheduler * scheduler);
    void removeScheduler(Scheduler * scheduler);
    friend struct Scheduler;//for addScheduler() and removeScheduler()

    std::list<Scheduler*> _schedulers;

    std::mutex _mutex;

    std::condition_variable _taskAddedCondition;
    bool notifiedTaskAdded = false;

    std::mutex _doTasksMutex;

    bool _threadRunning = false;

    std::thread _thread;
    std::thread::id _thisThreadId;

    Clock* _clock;
    static RealtimeClock _defaultClock;
};

}
#endif
