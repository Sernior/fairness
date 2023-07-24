#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <syncstream>
#include <thread>
#include <fair_shared_mutex.h>
#include <chrono>

 
int main()
{
    fsm::fair_shared_mutex m;
    auto acquireLockAndPrint = [&m](uint32_t threadPriority, uint32_t preLockTimer = 0, uint32_t postLockTimer = 0){
        auto threadID = std::this_thread::get_id();
        auto startTime = std::chrono::system_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(preLockTimer));
        m.lock(threadPriority);
        auto afterLockTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " entered critical section with priority: " << threadPriority
                << " (" << afterLockTime-startTime << ')' << '\n';
        std::this_thread::sleep_for(std::chrono::milliseconds(postLockTimer));
        auto afterComputationTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " finished its computations with priority: " << threadPriority
                << " (" << afterComputationTime-startTime << ')' << '\n';
        m.unlock();
    };

    std::thread thread1([&]{acquireLockAndPrint(0,0,2000);});
    std::thread thread2([&]{acquireLockAndPrint(1,200,400);});
    std::thread thread3([&]{acquireLockAndPrint(0,300,400);});
    std::thread thread4([&]{acquireLockAndPrint(2,100,200);});
    std::thread thread5([&]{acquireLockAndPrint(0,200,300);});
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    /*ThreadSafeCounter counter;
 
    auto increment_and_print = [&counter]()
    {
        for (int i{}; i != 3; ++i)
        {
            counter.increment();
            std::osyncstream(std::cout)
                << std::this_thread::get_id() << ' ' << counter.get() << '\n';
        }
    };
 
    std::thread thread1(increment_and_print);
    std::thread thread2(increment_and_print);
 
    thread1.join();
    thread2.join();*/
}