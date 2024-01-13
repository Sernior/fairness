/**
 * @file priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PRIORITY_MUTEX_CPL_HPP
#define BOOST_FAIRNESS_PRIORITY_MUTEX_CPL_HPP
#include <atomic>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/detail/wait_ops.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>
#include <boost/fairness/detail/wait_pool.hpp>
#include <boost/fairness/detail/pqlock.hpp>

namespace boost::fairness{

    /**
     * @brief The priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * \n 
     * It serves to protect shared data from simultaneous access by multiple threads and provides exclusive, non-recursive ownership semantics:
     *
     * * A thread _holds_ a priority_mutex from the moment it successfully calls either lock() or try_lock() until it calls unlock().
     * * While a thread owns a priority_mutex, all other threads attempting to acquire ownership will either block (in the case of lock()) or receive a false return value (with try_lock()).
     * * It's important to note that a thread should not possess the mutex prior to calling lock() or try_lock().
     *
     * The program's behavior becomes unpredictable when a priority_mutex is destroyed while it's still under the ownership of any threads or when a thread terminates while in possession of a priority_mutex.
     * \n
     * Additionally, undefined behavior can occur when you specify a number of priorities (N) that falls outside the valid range, which should be between 1 and [BOOST_FAIRNESS_MAXIMUM_PRIORITY](https://sernior.github.io/fairness/priority__t_8hpp.html#ab63ed35d4aa8f18cc832fecbf13ba0ae).
     * \n
     * The priority_mutex class complies with all [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType) requirements.
     * \n
     * Importantly, priority_mutex is neither copyable nor movable.
     *
     * @tparam N : represents the number of indexed priorities managed by the priority_mutex, ranging from 1 up to [BOOST_FAIRNESS_MAXIMUM_PRIORITY](https://sernior.github.io/fairness/priority__t_8hpp.html#ab63ed35d4aa8f18cc832fecbf13ba0ae).
     * @note priority_mutex is usually not accessed directly: unique_lock, lock_guard, or scoped_lock manage locking in a more exception-safe manner.
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class priority_mutex{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        priority_mutex() = default;

        /// @private
        priority_mutex(const priority_mutex&) = delete;

        /// @private
        priority_mutex& operator=(const priority_mutex&) = delete;

        /// @private
        priority_mutex(priority_mutex&&) = delete;

        /// @private
        priority_mutex& operator=(priority_mutex&&) = delete;

        /// @private
        ~priority_mutex() = default;

        /**
         * @brief Acquire the priority_mutex with a designated priority. If another thread has already obtained the lock, or if there are other threads waiting with higher priority, the lock() function will halt the current thread's execution until the lock is successfully obtained.
         * \n
         * However, if lock() is invoked by a thread that already possesses the mutex, this results in undefined behavior, potentially causing a program deadlock. An implementation capable of identifying this invalid usage is encouraged to raise a [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) exception with the error condition ```resource_deadlock_would_occur``` instead of risking a deadlock.
         * \n
         * Moreover, any previous unlock() operations on the same mutex are _synchronize-with_ this operation, following the semantics defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order).
         *
         * @param priority used to set a priority for this thread to aquire the lock.
         *
         * @warning Undefined behavior occurs if the priority falls outside the range from 0 to N-1.
         *
         * @return (none)
         *
         * @exception std::system_error Throws std::system_error in the event of errors, including those originating from the underlying operating system that would impede the lock() operation from meeting its specifications. It's important to note that no locking occurs if an exception is thrown.
         *
         * #### Example
         *
         * @code
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            lll_.lock(priority);
        }

        /**
         * @brief Unlocks the priority-mutex.
         * \n
         * It is essential for the priority-mutex to be held by the current thread of execution; otherwise, the outcome is undefined.
         * \n
         * This operation _synchronizes-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) any subsequent lock operation that obtains ownership of the same priority-mutex.
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
         * priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            lll_.unlock();

        }

        /**
         * @brief Try to acquire the unique ownership of the priority_mutex with a designated priority.
         * Returns immediately.
         * \n
         * On successful lock acquisition returns ```true```, otherwise returns ```false```. The return value must be used, otherwise the compiler is encouraged to issue a warning.
         * \n
         * This function is designed to potentially fail without a specific reason, even if the mutex is not presently locked by any other thread.
         * \n
         * However, invoking try_lock() when a thread already holds the mutex results in undefined behavior.
         * \n
         * If try_lock() returns ```true```, any preceding unlock() operation on the same mutex is _synchronized-with_ the current operation, as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order).
         * \n
         * It's important to note that if try_lock() returns ```false```, prior lock() calls do not establish a synchronization relationship with this operation.
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
         * priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

        /* The try_lock for the Craig algorithm will be implemented in a future update.

        [[nodiscard]] bool try_lock(Priority_t const priority = 0){ // try lock is difficult with the cpr logic
            
            lll_.try_lock(priority);
        }
        */

        private:

        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) detail::pqlock lll_;

    };

}
#endif // BOOST_FAIRNESS_PRIORITY_MUTEX_CPL_HPP