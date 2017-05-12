#include <iostream>
#include <thread>
#include <string>
#include <chrono>

#include "sms/io/Port.hpp"
#include "sms/QueuedScheduler.hpp"
#include "sms/io/ScheduledPortAdatper.hpp"

struct Ball {};

struct PingPonger : sms::BidirPort<Ball, Ball>
{
    PingPonger(const std::string & name = "") : _name(name) { }
	void process(const Ball& msg) override
	{
        //for (uint32_t i=0; i<50000; i++)
        //    d*=_count;

        if (_count++<_maxCount)
    		send(msg);
        else
        {
            std::cout << "Finished PingPonger " << _name << ", count=" << _count << std::endl;
            std::cout.flush();
            _finished = true;
        }
    }
    uint32_t _maxCount = 1000 * 100;
    uint32_t _count = 0;
    const std::string _name;
    bool _finished = false;
    uint64_t d = 1;
};

typedef sms::ScheduledPortAdapter<Ball, Ball> ScheduledPortAdapter;

int main(int argc, char *argv[])
{
    // Build the following stack
    //      -----------------------
    //     |       Scheduler       |
    //      -----------------------
    //       |                   |
    //  ------------        ------------
    // |            | Port |            |
    // | PingPonger |------| PingPonger |
    // |            |      |            |
    //  ------------        ------------
    {
        sms::Thread thread;
        sms::QueuedScheduler scheduler(&thread);

        //create ping-pongers
        PingPonger pingPonger1("1");
        PingPonger pingPonger2("2");

        ScheduledPortAdapter portPingPonger1(&pingPonger1, &scheduler);
        ScheduledPortAdapter portPingPonger2(&pingPonger2, &scheduler);

        //link offline controller port with charge point actions state machine
        portPingPonger1.setForeignPort(&portPingPonger2);
        portPingPonger2.setForeignPort(&portPingPonger1);

        std::ios_base::sync_with_stdio(false);

        std::cout << "Single scheduler test." << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();

        pingPonger1.process(Ball());
        //pingPonger1.process(Ball());
        //thread.doTasks();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "Ping-pong " << pingPonger1._count << " times with single scheduler took " << (double)time_us.count() / 1000000.0 << " secs to run." << std::endl;
        std::cout << "Ping-pong with single scheduler took " << (double)time_us.count() / pingPonger1._count << " us per circle." << std::endl; 
        std::cout << std::endl;
    }

    // Build the following stack
    //            ------------
    //           |   Thread   |
    //            ------------
    //           /            \
    //  ------------        -----------
    // | Scheduler  |      | Scheduler |
    //  ------------        -----------
    //       |                   |
    //  ------------        ------------
    // |            | Port |            |
    // | PingPonger |------| PingPonger |
    // |            |      |            |
    //  ------------        ------------
    {
        sms::Thread thread;
        sms::QueuedScheduler scheduler1(&thread);
        sms::QueuedScheduler scheduler2(&thread);

        //create ping-pongers
        PingPonger pingPonger1("1");
        PingPonger pingPonger2("2");

        ScheduledPortAdapter portPingPonger1(&pingPonger1, &scheduler1);
        ScheduledPortAdapter portPingPonger2(&pingPonger2, &scheduler2);

        //link offline controller port with charge point actions state machine
        portPingPonger1.setForeignPort(&portPingPonger2);
        portPingPonger2.setForeignPort(&portPingPonger1);

        auto start_time = std::chrono::high_resolution_clock::now();
        pingPonger1.process(Ball());
        //pingPonger1.process(Ball());
        //thread.doTasks();
 
        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

        std::cout << "Multiple scheduler test." << std::endl;
        std::cout << "Ping-pong " << pingPonger1._count << " times with multiple scheduler took " << (double)time_us.count() / 1000000.0 << " secs to run.\n";
        std::cout << "Ping-pong with multiple schedulers took " << (double)time_us.count() / pingPonger1._count << " us per circle." << std::endl;
        std::cout << std::endl;
    }

    // Build the following stack
    //  ------------        -----------
    // |  Thread    |      |   Thread  |
    //  ------------        -----------
    //       |                   |
    //  ------------        -----------
    // | Scheduler  |      | Scheduler |
    //  ------------        -----------
    //       |                   |
    //  ------------        ------------
    // |            | Port |            |
    // | PingPonger |------| PingPonger |
    // |            |      |            |
    //  ------------        ------------

    {
        sms::Thread thread1;
        sms::Thread thread2;
        sms::QueuedScheduler scheduler1(&thread1);
        sms::QueuedScheduler scheduler2(&thread2);

        //create ping-pongers
        PingPonger pingPonger1("1");
        PingPonger pingPonger2("2");

        ScheduledPortAdapter portPingPonger1(&pingPonger1, &scheduler1);
        ScheduledPortAdapter portPingPonger2(&pingPonger2, &scheduler2);

        //link offline controller port with charge point actions state machine
        portPingPonger1.setForeignPort(&portPingPonger2);
        portPingPonger2.setForeignPort(&portPingPonger1);

        std::ios_base::sync_with_stdio(false);

        std::cout << "Multiple scheduler and threads test." << std::endl;

        auto start_time = std::chrono::high_resolution_clock::now();

        thread1.start();
        thread2.start();

        pingPonger1.process(Ball());
        //pingPonger1.process(Ball());

        while (!pingPonger1._finished)
            std::this_thread::sleep_for(std::chrono::milliseconds(10));

        thread1.terminate();
        thread2.terminate();

        auto end_time = std::chrono::high_resolution_clock::now();
        auto time_us = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        std::cout << "Ping-pong " << pingPonger1._count << " times with multiple threads took " << (double)time_us.count() / 1000000.0 << " secs to run." << std::endl;
        std::cout << "Ping-pong with multiple threads took " << (double)time_us.count() / pingPonger1._count << " us per circle." << std::endl;
        std::cout << std::endl;
    }

    return 0;
}
