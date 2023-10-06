#ifndef BOOST_FAIRNESS_THREAD_UTILS
#define BOOST_FAIRNESS_THREAD_UTILS
#include "waiting_utils.hpp"

#define NOW std::chrono::high_resolution_clock::now()

namespace utils::thread{

    template <typename Lockable>
    void thread_function_milli(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_milli(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock();
        else lockable.lock(p);
        utils::waiting::busy_wait_milli(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }
    template <typename Lockable>
    void thread_function_micro(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_micro(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock();
        else lockable.lock(p);
        utils::waiting::busy_wait_micro(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    template <typename Lockable>
    void thread_function_nano(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_nano(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock();
        else lockable.lock(p);
        utils::waiting::busy_wait_nano(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    template <typename Lockable>
    void shared_thread_function_milli(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_milli(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock_shared();
        else lockable.lock_shared(p);
        utils::waiting::busy_wait_milli(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    template <typename Lockable>
    void shared_thread_function_micro(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_micro(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock_shared();
        else lockable.lock_shared(p);
        utils::waiting::busy_wait_micro(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    template <typename Lockable>
    void shared_thread_function_nano(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_nano(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.lock_shared();
        else lockable.lock_shared(p);
        utils::waiting::busy_wait_nano(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    template <typename Lockable>
    void try_thread_function_milli(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_milli(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock();
        else lockable.try_lock(p);
        utils::waiting::busy_wait_milli(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    template <typename Lockable>
    void try_thread_function_micro(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_micro(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock();
        else lockable.try_lock(p);
        utils::waiting::busy_wait_micro(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    template <typename Lockable>
    void try_thread_function_nano(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_nano(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock();
        else lockable.try_lock(p);
        utils::waiting::busy_wait_nano(criticalTime);
        lockable.unlock();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }

    template <typename Lockable>
    void try_shared_thread_function_milli(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_milli(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock_shared();
        else lockable.try_lock_shared(p);
        utils::waiting::busy_wait_milli(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_milli(postCriticalTime);
    }

    template <typename Lockable>
    void try_shared_thread_function_micro(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_micro(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock_shared();
        else lockable.try_lock_shared(p);
        utils::waiting::busy_wait_micro(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_micro(postCriticalTime);
    }

    template <typename Lockable>
    void try_shared_thread_function_nano(Lockable& lockable, int preCriticalTime, int criticalTime, int postCriticalTime, int p = 0){
        utils::waiting::busy_wait_nano(preCriticalTime);
        if constexpr (std::is_same_v<Lockable, std::mutex> || std::is_same_v<Lockable, std::shared_mutex>) lockable.try_lock_shared();
        else lockable.try_lock_shared(p);
        utils::waiting::busy_wait_nano(criticalTime);
        lockable.unlock_shared();
        utils::waiting::busy_wait_nano(postCriticalTime);
    }
}

#undef NOW
#endif // BOOST_FAIRNESS_THREAD_UTILS