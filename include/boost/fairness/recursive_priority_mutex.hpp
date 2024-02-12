//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file recursive_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the recursive_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).

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
     * The recursive_priority_mutex can be used to protect shared data from being simultaneously accessed by multiple threads.
     * \n
     * recursive_priority_mutex offers exclusive, recursive ownership semantics: 
     * 
     * * A calling thread _owns_ a recursive_priority_mutex for a period of time that starts when it successfully calls either lock() or try_lock() until it calls unlock(). During this period, the thread may make additional calls to lock() or try_lock(). The period of ownership ends when the thread makes a matching number of calls to unlock().
     * * When a thread owns a recursive_priority_mutex, all other threads will block (for calls to lock()) or receive a ```false``` return value (for try_lock()) if they attempt to claim ownership of the recursive_priority_mutex.
     * * The maximum number of times that a recursive_priority_mutex may be locked is unspecified, but after that number is reached, calls to lock will throw [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) and calls to try_lock() will return ```false```.
     * 
     * The behavior of a program is undefined if a recursive_priority_mutex is destroyed while still owned by some thread.
     * \n
     * Additionally, undefined behavior can occur when you specify a number of priorities (N) that falls outside the valid range, which should be between 1 and [BOOST_FAIRNESS_MAXIMUM_PRIORITY](https://sernior.github.io/fairness/priority__t_8hpp.html#ab63ed35d4aa8f18cc832fecbf13ba0ae).
     * \n 
     * The recursive_priority_mutex class satisfies all requirements of [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * 
     * @tparam N : represents the number of indexed priorities managed by the priority_mutex, ranging from 1 up to [BOOST_FAIRNESS_MAXIMUM_PRIORITY](https://sernior.github.io/fairness/priority__t_8hpp.html#ab63ed35d4aa8f18cc832fecbf13ba0ae).
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
         * @brief Acquire the recursive_priority_mutex with a designated priority. If another thread has already obtained the lock, or if there are other threads waiting with higher priority, the lock() function will halt the current thread's execution until the lock is successfully obtained.
         * \n
         * A thread may call lock on a recursive_priority_mutex repeatedly. Ownership will only be released after the thread makes a matching number of calls to unlock.
         * \n
         * The maximum number of levels of ownership is unspecified. An exception of type [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) will be thrown if this number is exceeded.
         * \n
         * Prior unlock() operations on the same mutex _synchronize-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) this operation.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * @warning Undefined behavior occurs if the priority falls outside the range from 0 to N-1.
         * 
         * @return (none)
         * 
         * @exception std::system_error Throws std::system_error when errors occur, including errors from the underlying operating system that would prevent lock from meeting its specifications. The mutex is not locked in the case of any exception being thrown.
         * 
         * #### Example
         * 
         * @code
         * #include <boost/fairness.hpp>
         * #include <iostream>
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
         * @brief Unlock the recursive_priority_mutex when its ownership level is ```1```, signifying that there was precisely one more call to lock() than there were calls to unlock() made by the current thread. In all other cases, it reduces the ownership level by 1.
         * \n
         * For proper functioning, the mutex must be currently held by the executing thread. Any other scenario results in undefined behavior.
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
         * #include <boost/fairness.hpp>
         * #include <iostream>
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

            waitingFlag_.reset_(p);

            internalMutex_.unlock();

            detail::notify_one(waitingFlag_[p]);
        }

        /**
         * @brief Try to acquire the unique ownership of the recursive_priority_mutex with a designated priority.
         * Returns immediately.
         * \n
         * On successful lock acquisition returns ```true```, otherwise returns ```false```. The return value must be used, otherwise the compiler is encouraged to issue a warning.
         * \n
         * This function is allowed to fail spuriously and return ```false``` even if the mutex is not currently locked by any other thread.
         * \n
         * A thread can make multiple consecutive calls to try_lock() on a recursive_priority_mutex. Each successful try_lock() call increments the ownership count, and the recursive_priority_mutex will only be released when the thread matches this count with an equivalent number of calls to unlock().
         * \n
         * The maximum number of ownership levels remains unspecified. If this number is exceeded, a call to try_lock() will return ```false```.
         * \n
         * If try_lock() returns ```true```, any preceding unlock() operation on the same mutex is _synchronized-with_ the current operation, as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order).
         * 
         * @param priority : used to set a priority for this thread to aquire the lock.
         * 
         * @warning Undefined behavior occurs if the priority falls outside the range from 0 to N-1.
         * 
         * @return [[[nodiscard](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)]] bool : ```true``` if the lock was acquired successfully, otherwise ```false```.
         * 
         * @exception Throws nothing.
         * 
         * #### Example
         *  
         * @code
         * #include <boost/fairness.hpp>
         * #include <iostream>
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

        detail::WaitPool<N> waitingFlag_{};

        std::array<Thread_cnt_t, N> waiters_{};

        std::thread::id owner_{};

        uint32_t recursionCounter_{};

        bool lock_not_owned_(){
            return std::thread::id() == owner_;
        }

        bool lock_owned_by_me_(){
            return owner_ == std::this_thread::get_id();
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
