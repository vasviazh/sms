#ifndef SMS_SCHEDULEDPORTADAPTER_H
#define SMS_SCHEDULEDPORTADAPTER_H

#include "sms/io/Port.hpp"
#include "sms/Task.hpp"
#include "sms/QueuedScheduler.hpp"

namespace sms {

template<typename TInput, typename TOutput>
class ScheduledPortAdapter : public BidirPort<TInput, TOutput>
{
public:
    ScheduledPortAdapter(BidirPort<TInput, TOutput> *adaptee, QueuedScheduler* scheduler)
        : _incomingPort(this), _scheduler(scheduler), _adaptee(adaptee)
    {
        _adaptee->setForeignPort(&_incomingPort);
    }

    virtual void process(const TInput& msg)
    {
        //std::unique_ptr<Task> task(new ScheduledPortTask<TInput>(this, msg));
        _scheduler->execTask(std::unique_ptr<Task>(new ScheduledPortTask(this, msg)));
    }
protected:
    struct ScheduledPortTask : public Task
    {
        ScheduledPortTask(ScheduledPortAdapter<TInput, TOutput>* port, const TInput & message)
            : _port(port), _message(new TInput(message))
        {}
        void exec() override
        {
            _port->_adaptee->process(*_message);
        }
        virtual ~ScheduledPortTask() = default;
    private:
        ScheduledPortAdapter<TInput, TOutput> *_port;
        std::unique_ptr<TInput> _message;
    };

    struct ReplyPort : Port<TOutput>
    {
        ReplyPort(ScheduledPortAdapter* parentPort)
            : _parentPort(parentPort)
        {}

        void process(const TOutput& message) override
        {
            _parentPort->send(message);
        }
        ScheduledPortAdapter* _parentPort;

    } _incomingPort;

private:
    QueuedScheduler* _scheduler;
    BidirPort<TInput, TOutput> *_adaptee;
};

}

#endif
