#ifndef SMS_TIMER_HPP
#define SMS_TIMER_HPP

#include <chrono>

namespace sms {

struct Clock
{
    typedef std::chrono::duration<double> TimePoint;
    typedef std::chrono::duration<double> Duration;
    virtual TimePoint now() = 0;
    virtual ~Clock() = default;
};

struct RealtimeClock : Clock
{
    RealtimeClock();
    virtual TimePoint now();
private:
    std::chrono::steady_clock::time_point _startTimePoint;
};

struct ManualClock : Clock
{
    ManualClock()
        : _currentTimePoint(0)
    {}
    void setTime(Clock::TimePoint time) {_currentTimePoint = time;}
    virtual TimePoint now() {return _currentTimePoint;}
private:
    Clock::TimePoint _currentTimePoint;
};

}
#endif
