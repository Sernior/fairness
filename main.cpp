#include <iostream>
#include <thread>
#include <priority_mutex.h>
#include <spinlock_priority_mutex.h>
#include <shared_priority_mutex.h>
#include <chrono>
#include <vector>
#include <algorithm>

static PrioSync::priority_mutex<8> m;
static PrioSync::spinlock_priority_mutex<4> ms;


void threadFunction(PrioSync::Priority_t prio) {

    ms.lock(prio);

    //std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::cout << "Thread : " << int(prio) << " is crit." << std::endl;

    
    ms.unlock();
}



int main()
{
    for (int i = 0; i < 4; i++){
        ms.lock();
        std::vector<std::thread> tp;
        // for (int i = 0; i < 4; i++){
        //     tp.push_back(std::thread(&threadFunction, i));
        //     tp.push_back(std::thread(&threadFunction, i));
        // }
        tp.push_back(std::thread(&threadFunction, 0));
        tp.push_back(std::thread(&threadFunction, 1));
        tp.push_back(std::thread(&threadFunction, 1));
        tp.push_back(std::thread(&threadFunction, 2));
        tp.push_back(std::thread(&threadFunction, 2));
        tp.push_back(std::thread(&threadFunction, 3));
        tp.push_back(std::thread(&threadFunction, 3));
        tp.push_back(std::thread(&threadFunction, 0));
        //std::this_thread::sleep_for(std::chrono::milliseconds(10));//make sure the threads lock themselves

        ms.unlock();
        for (auto& t : tp)
            t.join();
    }
    return 0;
}