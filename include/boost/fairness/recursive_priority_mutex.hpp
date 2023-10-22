/**
 * @file recursive_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the recursive_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
#include <thread>
#include <atomic>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/wait_ops.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>

namespace boost::fairness{

    #define WAIT 0
    #define PROCEED 1

    /**
     * @brief The recursive_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * \n 
     * The priority_mutex can be used to protect shared data from being simultaneously accessed by multiple threads.
     * \n
     * priority_mutex offers exclusive, non-recursive ownership semantics:
     * 
     * * A calling thread _owns_ a recursive_priority_mutex for a period of time that starts when it successfully calls either lock() or try_lock() until it calls unlock(). During this period, the thread may make additional calls to lock() or try_lock(). The period of ownership ends when the thread makes a matching number of calls to unlock().
     * * When a thread owns a recursive_priority_mutex, all other threads will block (for calls to lock()) or receive a ```false``` return value (for try_lock()) if they attempt to claim ownership of the recursive_priority_mutex.
     * * The maximum number of times that a recursive_priority_mutex may be locked is unspecified, but after that number is reached, calls to lock will throw [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) and calls to try_lock() will return ```false```.
     * 
     * The behavior of a program is undefined if a recursive_priority_mutex is destroyed while still owned by some thread. The recursive_priority_mutex class satisfies all requirements of [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * \n
     * 
     * @tparam N : number of 0 indexed priorities the recursive_priority_mutex manages, up to BOOST_FAIRNESS_MAXIMUM_PRIORITY.
     * @note priority_mutex is usually not accessed directly: unique_lock, lock_guard, or scoped_lock manage locking in a more exception-safe manner. 
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class recursive_priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        recursive_priority_mutex() = default;

        /// @private
        recursive_priority_mutex(const recursive_priority_mutex&) = delete;

        /// @private
        recursive_priority_mutex& operator=(const recursive_priority_mutex&) = delete;

        /// @private
        recursive_priority_mutex(recursive_priority_mutex&&) = delete;

        /// @private
        recursive_priority_mutex& operator=(recursive_priority_mutex&&) = delete;

        /// @private
        ~recursive_priority_mutex() = default;

        /**
         * @brief Lock the recursive_priority_mutex. If another thread has already locked the recursive_priority_mutex or other threads are waiting with higher priority, a call to lock will block execution until the lock is acquired.
         * \n
         * A thread may call lock on a recursive_priority_mutex repeatedly. Ownership will only be released after the thread makes a matching number of calls to unlock.
         * \n
         * The maximum number of levels of ownership is unspecified. An exception of type [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) will be thrown if this number is exceeded.
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
         * recursive_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            internalMutex_.lock(priority);

            ++waiters_[priority];
            
            for(;;){
                if ( 
                    lock_owned_by_me_() ||
                    (find_first_priority_() >= priority &&
                    lock_not_owned_())
                    ){

                        owner_ = std::this_thread::get_id();

                        --waiters_[priority];

                        ++recursionCounter_;

                        internalMutex_.unlock();

                        return;
                    }

                internalMutex_.unlock();

                detail::wait(waitingFlag_[priority], WAIT);

                internalMutex_.lock(priority);
            }
        }

        /**
         * @brief Unlocks the recursive_priority_mutex if its level of ownership is ```1``` (there was exactly one more call to lock() than there were calls to unlock() made by this thread), reduces the level of ownership by 1 otherwise.
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
         * @note recursive_priority_mutex is usually not called directly: unique_lock, lock_guard, are used to manage exclusive locking.
         * 
         * #### Example
         *  
         * @code
         * #define NUM_ARBITRARY_PRIORITIES 5
         * 
         * recursive_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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
            
            Priority_t p;

            internalMutex_.lock();

            p = find_first_priority_();

            --recursionCounter_;

            if (recursionCounter_ != 0)
            {
                internalMutex_.unlock();

                return;
            }

            owner_ = std::thread::id();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY){

                internalMutex_.unlock();

                return;
            }

            reset_(p); // maybe better before the unlock

            internalMutex_.unlock();

            detail::notify_one(waitingFlag_[p]);
        }

        /**
         * @brief Try to acquire the unique ownership of the recursive_priority_mutex.
         * Returns immediately. On successful lock acquisition returns ```true```, otherwise returns ```false```. The return value must be used, otherwise the compiler is encouraged to issue a warning.
         * \n
         * This function is allowed to fail spuriously and return ```false``` even if the mutex is not currently locked by any other thread.
         * \n
         * A thread may call try_lock() on a recursive_priority_mutex repeatedly. Successful calls to try_lock() increment the ownership count: the recursive_priority_mutex will only be released after the thread makes a matching number of calls to unlock().
         * \n
         * The maximum number of levels of ownership is unspecified. A call to try_lock() will return ```false``` if this number is exceeded.
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
         * recursive_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            internalMutex_.lock(priority);

            if (lock_owned_by_me_() ||
             (find_first_priority_() >= priority && lock_not_owned_())){

                owner_ = std::this_thread::get_id();

                ++recursionCounter_;

                internalMutex_.unlock();

                return true;
             }

            internalMutex_.unlock();

            return false;
        }

        private:
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) spinlock_priority_mutex<N> internalMutex_;
        std::array<Thread_cnt_t, N> waiters_;
        std::thread::id owner_{};
        uint32_t recursionCounter_{};
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) std::array<std::atomic<uint32_t>, N> waitingFlag_;

        bool lock_not_owned_(){
            return std::thread::id() == owner_;
        }

        bool lock_owned_by_me_(){
            return owner_ == std::this_thread::get_id();
        }

        void reset_(Priority_t p){
            for (Priority_t i = 0; i < N; ++i)
                waitingFlag_[i].store(WAIT);
            waitingFlag_[p].store(PROCEED);
        }

        Priority_t find_first_priority_(){
            for (Priority_t i = 0; i < N; ++i){
                if (waiters_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }
    };

    #undef WAIT
    #undef PROCEED
}
#endif // BOOST_FAIRNESS_RECURSIVE_PRIORITY_MUTEX_HPP
