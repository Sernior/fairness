/**
 * @file spinlock_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the spinlock_priority_mutex based on a scalable list base algorithm.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP

#ifdef BOOST_FAIRNESS_USE_TATAS_SPINLOCK
#include <boost/fairness/spinlock_priority_mutex_tatas.hpp>
#endif // BOOST_FAIRNESS_USE_TATAS_SPINLOCK

    /**
     * @brief The spinlock_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * \n
     * The spinlock_priority_mutex can be used to protect shared data from being simultaneously accessed by multiple threads.
     * \n
     * spinlock_priority_mutex offers exclusive, non-recursive ownership semantics:
     *
     * * A calling thread _owns_ a spinlock_priority_mutex from the time that it successfully calls either lock() or try_lock() until it calls unlock().
     * * When a thread owns a spinlock_priority_mutex, all other threads will block (for calls to lock()) or receive a ```false``` return value (for try_lock()) if they attempt to claim ownership of the spinlock_priority_mutex.
     * * A calling thread must not own the mutex prior to calling lock() or try_lock().
     *
     * The behavior of a program is undefined if a spinlock_priority_mutex is destroyed while still owned by any threads, or a thread terminates while owning a spinlock_priority_mutex. The spinlock_priority_mutex class satisfies all requirements of [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * \n
     * spinlock_priority_mutex is neither copyable nor movable.
     *
     * @tparam N : number of 0 indexed priorities the spinlock_priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     * @note spinlock_priority_mutex is usually not accessed directly: unique_lock, lock_guard, or scoped_lock manage locking in a more exception-safe manner.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class spinlock_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        spinlock_priority_mutex() = default;

        /// @private
        spinlock_priority_mutex(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(const spinlock_priority_mutex&) = delete;

        /// @private
        spinlock_priority_mutex(spinlock_priority_mutex&&) = delete;

        /// @private
        spinlock_priority_mutex& operator=(spinlock_priority_mutex&&) = delete;

        /// @private
        ~spinlock_priority_mutex() = default;

        /**
         * @brief Lock the spinlock_priority_mutex. If another thread has already locked the mutex or other threads are waiting with higher priority, a call to lock will block execution until the lock is acquired.
         * \n
         * If lock() is called by a thread that already owns the mutex, the behavior is undefined: for example, the program _may_ deadlock. An implementation that can detect the invalid usage is encouraged to throw a [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with error condition ```resource_deadlock_would_occur``` instead of deadlocking.
         * \n
         * Prior unlock() operations on the same mutex _synchronize-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) this operation.
         *
         * @param priority used to set a priority for this thread to aquire the lock.
         *
         * @return (none)
         *
         * @exception std::system_error Throws std::system_error when errors occur, including errors from the underlying operating system that would prevent lock from meeting its specifications. The mutex is not locked in the case of any exception being thrown.
         *
         * #### Example
         *
         * @code
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * spinlock_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
         *
         * void my_function(uint8_t prio = 0) {
         *      //...some code.
         *      m.lock(prio));
         *      std::cout << "thread with prio : " << prio << "\n";
         *      //...some code.
         * }
         * @endcode
         *
         * Possible output:
         *
         * \code{.cpp}
         * thread with prio : 0
         *
         * \endcode
         */
        void lock(Priority_t const priority = 0){
            Priority_t localCurrentPriority = currentPriority_.load(std::memory_order_relaxed);
            waiters_[priority].fetch_add(1, std::memory_order_relaxed);
            while ( 
                (localCurrentPriority < priority || !currentPriority_.compare_exchange_weak(localCurrentPriority, priority, std::memory_order_relaxed)) ||
                (lockOwned_.test_and_set(std::memory_order_acquire))
            ){
                localCurrentPriority = currentPriority_;
            }
            waiters_[priority].fetch_sub(1, std::memory_order_relaxed);
        }

        /**
         * @brief Unlocks the mutex.
         * \n
         * The mutex must be locked by the current thread of execution, otherwise, the behavior is undefined.
         * \n
         * This operation _synchronizes-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) any subsequent lock operation that obtains ownership of the same mutex.
         *
         * @param (none)
         *
         * @return (none)
         *
         * @exception Throws nothing.
         *
         * #### Example
         *
         * @code
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * spinlock_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
         *
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      std::cout << "mutex unlocked \n";
         *      //...some code.
         * }
         * @endcode
         *
         * Possible output:
         *
         * \code{.cpp}
         * mutex unlocked
         *
         * \endcode
         */
        void unlock(){
            currentPriority_.store(find_first_priority_(), std::memory_order_relaxed);
            lockOwned_.clear(std::memory_order_release);
        }

        /**
         * @brief Try to acquire the unique ownership of the spinlock_priority_mutex.
         * Returns immediately. On successful lock acquisition returns ```true```, otherwise returns ```false```. The return value must be used, otherwise the compiler is encouraged to issue a warning.
         * \n
         * This function is allowed to fail spuriously and return ```false``` even if the mutex is not currently locked by any other thread.
         * \n
         * If try_lock() is called by a thread that already owns the mutex, the behavior is undefined.
         * \n
         * Prior unlock() operation on the same mutex _synchronizes-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) this operation if it returns ```true```. Note that prior lock() does not synchronize with this operation if it returns ```false```.
         *
         * @param priority : used to set a priority for this thread to aquire the lock.
         *
         * @return [[[nodiscard](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)]] bool : ```true``` if the lock was acquired successfully, otherwise ```false```.
         *
         * @exception Throws nothing.
         *
         * #### Example
         *
         * @code
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * spinlock_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
         *
         * void my_function(uint8_t prio = 0) {
         *      //...some code.
         *      if (m.try_lock(prio)) {
         *          std::cout << "thread with prio : " << prio << "\n";
         *      }
         *      //...some code.
         * }
         * @endcode
         *
         * Possible output:
         *
         * \code{.cpp}
         * thread with prio : 0
         *
         * \endcode
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){
            return (currentPriority_.load(std::memory_order_relaxed) >= priority && !lockOwned_.test_and_set(std::memory_order_acquire));
        }

        private:
        std::array<std::atomic<Thread_cnt_t>, N> waiters_;
        std::atomic<Priority_t> currentPriority_{BOOST_FAIRNESS_MAXIMUM_PRIORITY};
        std::atomic_flag lockOwned_;

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }
    };
}
#endif // BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP
