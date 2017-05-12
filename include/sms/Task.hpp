#ifndef SMS_TASK_HPP
#define SMS_TASK_HPP

namespace sms {

struct Task
{
    virtual void exec() = 0;
    virtual ~Task() = default;
};

}
#endif
