//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file slim_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the slim_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
#include <atomic>
#include <cstring>
#include <array>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/config.hpp>
#include <boost/fairness/detail/control_block_t.hpp>

namespace boost::fairness{

    /**
     * @brief 
     * @private
     * @tparam ool 
     */
    template<bool> struct Range;
    /**
     * @brief 
     * @private
     * @tparam N 
     * @tparam typename 
     */
    template<size_t N = 1, typename = Range<true> >
    struct slim_priority_mutex{};

    /**
     * @brief The slim_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * \n
     * The slim_priority_mutex can be used to protect shared data from being simultaneously accessed by multiple threads.
     * \n
     * slim_priority_mutex offers exclusive, non-recursive ownership semantics:
     *
     * * A calling thread _owns_ a slim_priority_mutex from the time that it successfully calls either lock() or try_lock() until it calls unlock().
     * * When a thread owns a slim_priority_mutex, all other threads will block (for calls to lock()) or receive a ```false``` return value (for try_lock()) if they attempt to claim ownership of the slim_priority_mutex.
     * * A calling thread must not own the mutex prior to calling lock() or try_lock().
     *
     * The behavior of a program is undefined if a slim_priority_mutex is destroyed while still owned by any threads, or a thread terminates while owning a slim_priority_mutex. The slim_priority_mutex class satisfies all requirements of [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * \n
     * slim_priority_mutex is neither copyable nor movable.
     *
     * @tparam N : number of 0 indexed priorities the slim_priority_mutex manages, up to BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES.
     * @note slim_priority_mutex is usually not accessed directly: unique_lock, lock_guard, or scoped_lock manage locking in a more exception-safe manner.
     */
    template<size_t N>
    class slim_priority_mutex<N, Range<(1 <= N && N <= BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES)>>{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        slim_priority_mutex() = default;

        /// @private
        slim_priority_mutex(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex& operator=(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex(slim_priority_mutex&&) = delete;

        /// @private
        slim_priority_mutex& operator=(slim_priority_mutex&&) = delete;

        /// @private
        ~slim_priority_mutex() = default;

        /**
         * @brief Lock the slim_priority_mutex. If another thread has already locked the mutex or other threads are waiting with higher priority, a call to lock will block execution until the lock is acquired.
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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            control_block_64b_t localCtrl = ctrl_.load();

            // try to increment the waiters counter in this priority
            for(;;){
                // if we risk an overflow lets spin instead
                if (localCtrl.priority_[priority] == uint8_t(-1)){
                    localCtrl = ctrl_.load();
                    continue;
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.increasePriority(priority))){
                    break;
                }
            }
            // if we can proceed we do and decrement the counter otherwise we wait on an atomic_flag
            for(;;){
                localCtrl = ctrl_.load();
                if (!localCtrl.isOwned_() && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned())){
                    break;
                }
                waitingFlags_[priority].wait(false);
            }

            localCtrl = ctrl_.load();

            for(;;){
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.decreasePriority(priority))){
                    break;
                }
            }

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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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
            Priority_t localFirstPriority;
            control_block_64b_t localCtrl;
            for (;;){
                reset_();
                localCtrl = ctrl_.load();
                localFirstPriority = find_first_priority_(localCtrl);
                if (localFirstPriority < N){
                    waitingFlags_[localFirstPriority].test_and_set();
                    waitingFlags_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.setPriority(localFirstPriority)))
                    break;
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex.
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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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
            control_block_64b_t localCtrl = ctrl_.load();
            return !localCtrl.isOwned_() && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned());
        }

        private:
        std::array<std::atomic_flag, N> waitingFlags_{};
        std::atomic<control_block_64b_t> ctrl_;

        void reset_(){ // there probably is a much better way to do this
            std::memset(&waitingFlags_, 0b00000000, N); // maybe undefined because lock is reading on the waits? Should be ok.
        }

        Priority_t find_first_priority_(control_block_64b_t const& ctrl){
            for (Priority_t i = 0; i < N; ++i){
                if (ctrl.priority_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES;
        }

    };

#ifdef BOOST_FAIRNESS_HAS_DWCAS

    /**
     * @brief The slim_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * \n
     * The slim_priority_mutex can be used to protect shared data from being simultaneously accessed by multiple threads.
     * \n
     * slim_priority_mutex offers exclusive, non-recursive ownership semantics:
     *
     * * A calling thread _owns_ a slim_priority_mutex from the time that it successfully calls either lock() or try_lock() until it calls unlock().
     * * When a thread owns a slim_priority_mutex, all other threads will block (for calls to lock()) or receive a ```false``` return value (for try_lock()) if they attempt to claim ownership of the slim_priority_mutex.
     * * A calling thread must not own the mutex prior to calling lock() or try_lock().
     *
     * The behavior of a program is undefined if a slim_priority_mutex is destroyed while still owned by any threads, or a thread terminates while owning a slim_priority_mutex. The slim_priority_mutex class satisfies all requirements of [Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * \n
     * slim_priority_mutex is neither copyable nor movable.
     *
     * @tparam N : number of 0 indexed priorities the slim_priority_mutex manages, up to 15.
     * @note slim_priority_mutex is usually not accessed directly: unique_lock, lock_guard, or scoped_lock manage locking in a more exception-safe manner.
     */
    template<size_t N>
    class slim_priority_mutex<N, Range<(BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES < N && N <= BOOST_FAIRNESS_SPM128B_SUPPORTED_PRIORITIES)>>{

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        slim_priority_mutex() = default;

        /// @private
        slim_priority_mutex(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex& operator=(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex(slim_priority_mutex&&) = delete;

        /// @private
        slim_priority_mutex& operator=(slim_priority_mutex&&) = delete;

        /// @private
        ~slim_priority_mutex() = default;

        /**
         * @brief Lock the slim_priority_mutex. If another thread has already locked the mutex or other threads are waiting with higher priority, a call to lock will block execution until the lock is acquired.
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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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

            control_block_128b_t localCtrl = ctrl_.load();

            // try to increment the waiters counter in this priority
            for(;;){
                // if we risk an overflow lets spin instead
                if (localCtrl.priority_[priority] == uint8_t(-1)){
                    localCtrl = ctrl_.load();
                    continue;
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.increasePriority(priority))){
                    break;
                }
            }
            // if we can proceed we do and decrement the counter otherwise we wait on an atomic_flag
            for(;;){
                localCtrl = ctrl_.load();
                if (!localCtrl.isOwned_() && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned())){
                    break;
                }
                waitingFlags_[priority].wait(false);
            }

            localCtrl = ctrl_.load();

            for(;;){
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.decreasePriority(priority))){
                    break;
                }
            }

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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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
            Priority_t localFirstPriority;
            control_block_128b_t localCtrl;
            for (;;){
                reset_();
                localCtrl = ctrl_.load();
                localFirstPriority = find_first_priority_(localCtrl);
                if (localFirstPriority < N){
                    waitingFlags_[localFirstPriority].test_and_set();
                    waitingFlags_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.setPriority(localFirstPriority)))
                    break;
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex.
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
         * #include <boost/fairness.hpp>
         * #include <iostream>
         * #define NUM_ARBITRARY_PRIORITIES 5
         *
         * slim_priority_mutex<NUM_ARBITRARY_PRIORITIES> m;
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
            control_block_128b_t localCtrl = ctrl_.load();
            return !localCtrl.isOwned_() && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned());
        }

        private:
        std::array<boost::atomic_flag, N> waitingFlags_{};
        boost::atomic<control_block_128b_t> ctrl_;

        void reset_(){ // there probably is a much better way to do this
            std::memset(&waitingFlags_, 0b00000000, N); // maybe undefined because lock is reading on the waits? Should be ok.
        }

        Priority_t find_first_priority_(control_block_128b_t const& ctrl){
            for (Priority_t i = 0; i < N; ++i){
                if (ctrl.priority_[i] > 0)
                    return i;
            }
            return BOOST_FAIRNESS_SPM128B_SUPPORTED_PRIORITIES;
        }

    };

#endif // BOOST_FAIRNESS_HAS_DWCAS

}
#endif // BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
