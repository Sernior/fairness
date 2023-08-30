#include <iostream>
#include <thread>
#include <spinlock_priority_mutex.h>
#include <chrono>
#include <vector>
#include <algorithm>

static PrioSync::spinlock_priority_mutex<4> ms;


void threadFunction(PrioSync::Priority_t prio) {

    ms.lock(prio);

    std::cout << "Thread : " << int(prio) << " is crit." << std::endl;
    
    ms.unlock();
}



int main()
{
    ms.lock();
    std::vector<std::thread> tp;
     for (int i = 0; i < 4; i++){
        tp.emplace_back(threadFunction, i);
     }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));//make sure the threads lock themselves
    ms.unlock();
    for (auto& t : tp)
        t.join();
    return 0;
}