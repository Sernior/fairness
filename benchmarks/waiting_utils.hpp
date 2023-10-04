#ifndef BOOST_FAIRNESS_WAITING_UTILS
#define BOOST_FAIRNESS_WAITING_UTILS
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()

namespace waiting::utils{
    /*
    std::this_thread::sleep_for is too imprecise, and also I could make a bunch of variables during busy waiting to simulate heated cpu caches.
    */

    static void busy_wait_micro(uint32_t microseconds){
        auto begin = NOW;
        for(;std::chrono::duration_cast<std::chrono::microseconds>(NOW - begin).count() < microseconds;)
            continue;
    }

    static void busy_wait_milli(uint32_t milliseconds){
        auto begin = NOW;
        for(;std::chrono::duration_cast<std::chrono::milliseconds>(NOW - begin).count() < milliseconds;)
            continue;
    }

    static void busy_wait_nano(uint32_t nanoseconds){
        auto begin = NOW;
        for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < nanoseconds;)
            continue;
    }
}

#undef NOW
#endif // BOOST_FAIRNESS_WAITING_UTILS