#ifndef BOOST_FAIRNESS_WAITING_UTILS
#define BOOST_FAIRNESS_WAITING_UTILS
#include <chrono>

#define NOW std::chrono::high_resolution_clock::now()

namespace utils::waiting{
    enum class WaitingLevel { Milli, Micro, Nano };

    /*
    std::this_thread::sleep_for is too imprecise, and also I could make a bunch of variables during busy waiting to simulate heated cpu caches.
    */

    template <WaitingLevel level>
    void busy_wait(uint32_t utime) {
        auto begin = NOW;
        if constexpr (level == WaitingLevel::Milli)
            for(;std::chrono::duration_cast<std::chrono::milliseconds>(NOW - begin).count() < utime;)
                continue;
        if constexpr (level == WaitingLevel::Micro)
            for(;std::chrono::duration_cast<std::chrono::microseconds>(NOW - begin).count() < utime;)
                continue;
        if constexpr (level == WaitingLevel::Nano)
            for(;std::chrono::duration_cast<std::chrono::nanoseconds>(NOW - begin).count() < utime;)
                continue;
    }
}

#undef NOW
#endif // BOOST_FAIRNESS_WAITING_UTILS