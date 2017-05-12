#ifndef SMS_PORT_H
#define SMS_PORT_H

namespace sms
{

template<typename TInput>
class Port
{
public:

    using InputType = TInput;

    virtual void process(const InputType& Msg) = 0;

    virtual ~Port() = default;
};

template<typename TInput, typename TOutput>
class BidirPort : public Port<TInput>
{
public:
    using ForeignPortType = BidirPort<TOutput, TInput>;
    using OutputType = TOutput;

    BidirPort() {}

    BidirPort(BidirPort<TOutput, TInput>* foreignPort)
        : _foreignPort(foreignPort)
    {
        foreignPort->setForeignPort(this);
    }

    virtual void send(const TOutput& message) const
    {
        if (_foreignPort)
            _foreignPort->process(message);
        else
            throw std::runtime_error("foreign port not defined");
    }

//protected:
    Port<TOutput>* getForeignPort() {return _foreignPort;}
    void setForeignPort(Port<TOutput>* foreignPort) {_foreignPort = foreignPort;}

private:
    Port<TOutput>* _foreignPort = nullptr;
};

}

#endif // SMS_IO_PORT_H
