#include "sms/Timer.hpp"

namespace sms
{

RealtimeClock::RealtimeClock()
    : _startTimePoint(std::chrono::steady_clock::now())
{

}

Clock::TimePoint RealtimeClock::now()
{
    return std::chrono::steady_clock::now() - _startTimePoint;
}


}
