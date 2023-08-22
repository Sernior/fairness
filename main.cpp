#include <iostream>
#include <thread>
#include <priority_mutex.h>
#include <shared_priority_mutex.h>
#include <chrono>
#include <vector>
#include <algorithm>

static PrioSync::priority_mutex<8> m;


void threadFunction(PrioSync::Priority_t prio) {
    m.lock(prio);
    std::cout << "Thread with priority : " << int(prio) << " is running." << std::endl;
    m.unlock();
}


int main()
{
    m.lock();
    std::vector<std::thread> tp;
    for (int i = 0; i < 4; i++){
        tp.push_back(std::thread(&threadFunction, i));
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(10));//make sure the threads lock themselves

    m.unlock();
    for (auto& t : tp)
        t.join();

    return 0;
}