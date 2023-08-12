#include <iostream>
#include <syncstream>
#include <thread>
#include <fair_shared_mutex.h>
#include <priority_mutex.h>
#include <chrono>
#include <vector>
#include <algorithm>
 
int main()
{
    PrioSync::priority_mutex<9> m1;
    fsm::fair_shared_mutex m;


    auto startTime = std::chrono::system_clock::now();
    std::vector<std::thread::id> res;
    auto acquireLockAndPrint = [&m1, &startTime,&res](uint32_t threadPriority = 0, uint32_t preLockTimer = 0, uint32_t criticalSectionTimer = 0, uint32_t postUnlockTimer = 0){
        auto threadID = std::this_thread::get_id();
        std::this_thread::sleep_for(std::chrono::microseconds(preLockTimer));
        m1.lock(threadPriority);
        auto afterLockTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " entered critical section with priority: " << threadPriority
                << " (" << (afterLockTime-startTime).count() << " ns)" << '\n';
        std::this_thread::sleep_for(std::chrono::microseconds(criticalSectionTimer));
        auto afterComputationTime = std::chrono::system_clock::now();
        std::osyncstream(std::cout) << "Thread: "
                << threadID << " finished its computations with priority: " << threadPriority
                << " (" << (afterComputationTime-startTime).count() << " ns)" << '\n';
        res.push_back(threadID);
        m1.unlock();
        std::this_thread::sleep_for(std::chrono::microseconds(postUnlockTimer));
    };

    std::thread thread1([&]{acquireLockAndPrint(0,0,2000);});
    std::thread thread2([&]{acquireLockAndPrint(1,700,400,200);});
    std::thread thread3([&]{acquireLockAndPrint(2,300,400,300);});
    std::thread thread4([&]{acquireLockAndPrint(3,100,200,100);});
    std::thread thread5([&]{acquireLockAndPrint(5,1200,300,200);});
    std::thread thread6([&]{acquireLockAndPrint(6,300,300,300);});
    std::thread thread7([&]{acquireLockAndPrint(7,100,100,100);});
    std::thread thread8([&]{acquireLockAndPrint(8,200,200,200);});
    thread1.join();
    thread2.join();
    thread3.join();
    thread4.join();
    thread5.join();
    thread6.join();
    thread7.join();
    thread8.join();
    bool is_sorted = std::is_sorted(res.begin()+1, res.end());
    if (is_sorted){
        std::cout << "Correct";
        return 0;
    }
    for (auto& r : res){
        std::cout << r << "  ";
    }
}