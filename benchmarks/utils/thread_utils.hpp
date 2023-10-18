/**
 * @file #TODO.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias #TODO.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_THREAD_UTILS
#define BOOST_FAIRNESS_THREAD_UTILS
#include <mutex>
#include <shared_mutex>
#include "waiting_utils.hpp"

#define NOW std::chrono::high_resolution_clock::now()

namespace utils::thread{
    enum class LockLevel { Unique, Shared, Unique_Try, Shared_Try };

    template <LockLevel lock_level, utils::waiting::WaitingLevel waiting_level, typename Lockable>
    void thread_function(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0) {
        if constexpr (lock_level == LockLevel::Unique) {
            utils::waiting::busy_wait<waiting_level>(preCriticalTime);
            if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex> || std::is_same_v<Lockable, std::recursive_mutex>) lockable.lock();
            else lockable.lock(p);
            utils::waiting::busy_wait<waiting_level>(criticalTime);
            lockable.unlock();
            utils::waiting::busy_wait<waiting_level>(postCriticalTime);
        }
        if constexpr (lock_level == LockLevel::Shared) {
            utils::waiting::busy_wait<waiting_level>(preCriticalTime);
            if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex> || std::is_same_v<Lockable, std::recursive_mutex>) lockable.lock_shared();
            else lockable.lock_shared(p);
            utils::waiting::busy_wait<waiting_level>(criticalTime);
            lockable.unlock_shared();
            utils::waiting::busy_wait<waiting_level>(postCriticalTime);
        }
        if constexpr (lock_level == LockLevel::Unique_Try) {
            utils::waiting::busy_wait<waiting_level>(preCriticalTime);
            if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex> || std::is_same_v<Lockable, std::recursive_mutex>) lockable.try_lock();
            else lockable.try_lock(p);
            utils::waiting::busy_wait<waiting_level>(criticalTime);
            lockable.unlock();
            utils::waiting::busy_wait<waiting_level>(postCriticalTime);
        }
        if constexpr (lock_level == LockLevel::Shared_Try) {
            utils::waiting::busy_wait<waiting_level>(preCriticalTime);
            if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex> || std::is_same_v<Lockable, std::recursive_mutex>) lockable.try_lock_shared();
            else lockable.try_lock_shared(p);
            utils::waiting::busy_wait<waiting_level>(criticalTime);
            lockable.unlock_shared();
            utils::waiting::busy_wait<waiting_level>(postCriticalTime);
        }
    }
}

#undef NOW
#endif // BOOST_FAIRNESS_THREAD_UTILS