/**
 * @file shared_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the shared_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
#include <array>
#include <atomic>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>
#include <boost/fairness/detail/wait_ops.hpp>

namespace boost::fairness{

    #define WAIT 0
    #define PROCEED 1

    /**
     * @brief The shared_priority_mutex is an advanced synchronization mechanism that enhances the traditional shared_mutex by introducing a priority-based approach.
     * \n 
     * The shared_priority_mutex class is a synchronization primitive that can be used to protect shared data from being simultaneously accessed by multiple threads. In contrast to other mutex types which facilitate exclusive access, a shared_mutex has two levels of access:
     * 
     * * _shared_ - several threads can share ownership of the same mutex;
     * * _exclusive_ - only one thread can own the mutex.
     * 
     * If one thread has acquired the _exclusive_ lock (through lock(), try_lock()), no other threads can acquire the lock (including the _shared_).
     * \n 
     * If one thread has acquired the _shared_ lock (through lock_shared(), try_lock_shared()), no other thread can acquire the _exclusive_ lock, but can acquire the _shared_ lock.
     * \n 
     * Only when the _exclusive_ lock has not been acquired by any thread, the _shared_ lock can be acquired by multiple threads.
     * \n
     * Within one thread, only one lock (_shared_ or _exclusive_) can be acquired at the same time.
     * \n
     * Shared mutexes are especially useful when shared data can be safely read by any number of threads simultaneously, but a thread may only write the same data when no other thread is reading or writing at the same time.
     * \n 
     * The shared_priority_mutex class satisfies all requirements of [SharedMutex](https://en.cppreference.com/w/cpp/named_req/SharedMutex) and [StandardLayoutType](https://en.cppreference.com/w/cpp/named_req/StandardLayoutType).
     * 
     * @tparam N : number of 0 indexed priorities the shared_priority_mutex manages, up to [BOOST_FAIRNESS_MAXIMUM_PRIORITY](https://sernior.github.io/fairness/priority__t_8hpp.html#ab63ed35d4aa8f18cc832fecbf13ba0ae).
     */
    template<size_t N = 1>
    requires (N >= 1 && N <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    class shared_priority_mutex{

        using Thread_cnt_t = uint32_t;

        struct threadPriority{
            Thread_cnt_t writers_waiting{};
            Thread_cnt_t readers_waiting{};
        };

        template<size_t S = 1>
        requires (S >= 1 && S <= BOOST_FAIRNESS_MAXIMUM_PRIORITY)
        struct waitingFlags{
            std::array<std::atomic<uint32_t>, S> writers;
            std::array<std::atomic<uint32_t>, S> readers;
        };

        public:

        /// @private
        shared_priority_mutex() = default;

        /// @private
        shared_priority_mutex(const shared_priority_mutex&) = delete;

        /// @private
        shared_priority_mutex& operator=(const shared_priority_mutex&) = delete;

        /// @private
        shared_priority_mutex(shared_priority_mutex&&) = delete;

        /// @private
        shared_priority_mutex& operator=(shared_priority_mutex&&) = delete;

        /// @private
        ~shared_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already owned or other threads are waiting with higher priority.
         * \n 
         * If another thread is holding an exclusive lock() or a shared_lock() on the same shared_priority_mutex the a call to lock will block execution until all such locks are released. While shared_priority_mutex is locked in an exclusive mode, no other lock() of any kind can also be held.
         * \n 
         * If lock is called by a thread that already owns the shared_mutex in any mode (exclusive or shared), the behavior is undefined. A prior unlock() operation on the same mutex _synchronizes-with_ (as defined in [std::memory_order](https://en.cppreference.com/w/cpp/atomic/memory_order)) this operation.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * @return none.
         * @exception Throws [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) when errors occur, including errors from the underlying operating system that would prevent lock from meeting its specifications. The mutex is not locked in the case of any exception being thrown.
         * @note lock() is usually not called directly: boost::fairness::unique_lock, boost::fairness::scoped_lock, and boost::fairness::lock_guard are used to manage exclusive locking.
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){

            internalMutex_.lock(priority);

            ++priorities_[priority].writers_waiting;

            ++totalWritersWaiting_;

            for (;;)
            {

                if (
                    totalCurrentReaders_ == 0 &&
                    !lockOwned_ &&
                    find_first_priority_() >= priority
                ){
                    --priorities_[priority].writers_waiting;

                    --totalWritersWaiting_;

                    lockOwned_ = true;

                    internalMutex_.unlock();

                    return;
                }

                internalMutex_.unlock();

                detail::wait(waitingFlags_.writers[priority], WAIT);

                internalMutex_.lock(priority);

            }
        }

        /**
         * @brief Release the shared_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){

            Priority_t p;

            internalMutex_.lock();

            lockOwned_ = false;

            p = find_first_priority_();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY)
            {
                internalMutex_.unlock();

                return;
            }

            if (totalWritersWaiting_ == 0)
            {
                allow_all_readers_();

                internalMutex_.unlock();

                notify_all_readers_();

                return;
            }

            reset_(p);

            internalMutex_.unlock();

            notify_priority_(p);

        }

        /**
         * @brief Try to acquire the unique ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){

            internalMutex_.lock(priority);

            if (lockOwned_ ||
            totalCurrentReaders_ > 0 ||
            find_first_priority_() < priority)
            {

                internalMutex_.unlock();

                return false;
            }

            lockOwned_ = true;

            internalMutex_.unlock();

            return true;
        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, blocking the thread if the shared_priority_mutex was already uniquely owned or if another thread is waiting for unique ownership with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock_shared(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock_shared(Priority_t priority = 0){
            internalMutex_.lock(priority);

            ++priorities_[priority].readers_waiting;

            for(;;){

                if (!lockOwned_ && find_first_priority_with_writers_() >= priority)
                {
                    ++totalCurrentReaders_;

                    --priorities_[priority].readers_waiting;

                    internalMutex_.unlock();

                    return;
                }

                internalMutex_.unlock();

                detail::wait(waitingFlags_.readers[priority], WAIT);

                internalMutex_.lock(priority);

            }
        }

        /**
         * @brief Release the shared_priority_mutex from shared ownership.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock_shared();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock_shared(){

            Priority_t p;

            internalMutex_.lock();

            --totalCurrentReaders_;

            p = find_first_priority_();

            if (p == BOOST_FAIRNESS_MAXIMUM_PRIORITY)
            {
                internalMutex_.unlock();

                return;
            }

            if (totalWritersWaiting_ == 0)
            {
                allow_all_readers_();

                internalMutex_.unlock();

                notify_all_readers_();

                return;
            }

            if (totalCurrentReaders_ == 0)
            {
                reset_(p);

                internalMutex_.unlock();

                notify_priority_(p);

                return;
            }

            internalMutex_.unlock();

        }

        /**
         * @brief Try to acquire the shared ownership of the shared_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock_shared.
         * 
         * \code{.cpp}
         * shared_priority_mutex<10> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock_shared(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock_shared(Priority_t priority = 0){
            internalMutex_.lock(priority);

            if (lockOwned_ || find_first_priority_with_writers_() < priority){

                internalMutex_.unlock();

                return false;
            }

            ++totalCurrentReaders_;

            internalMutex_.unlock();

            return true;
        }

        private:

        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) spinlock_priority_mutex<N> internalMutex_;
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) waitingFlags<N> waitingFlags_;
        std::array<threadPriority, N> priorities_;
         Thread_cnt_t totalCurrentReaders_{};
        Thread_cnt_t totalWritersWaiting_{};
        bool lockOwned_{};

        Priority_t find_first_priority_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting+priorities_[i].readers_waiting > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }

        Priority_t find_first_priority_with_writers_() const {
            for (Priority_t i = 0; i <  N; ++i){
                if (priorities_[i].writers_waiting > 0)
                    return i;
            }
            return BOOST_FAIRNESS_MAXIMUM_PRIORITY;
        }

        void notify_all_readers_(){
            for (Priority_t i = 0; i < N; ++i){
                waitingFlags_.readers[i].store(PROCEED);
                detail::notify_all(waitingFlags_.readers[i]);
            }
        }

        void notify_priority_(Priority_t const p){
            detail::notify_one(waitingFlags_.writers[p]);
            detail::notify_all(waitingFlags_.readers[p]);
        }

        void reset_(Priority_t const p){
            for (Priority_t i = 0; i <  N; ++i){
                waitingFlags_.writers[i].store(WAIT);
                waitingFlags_.readers[i].store(WAIT);
            }
            waitingFlags_.writers[p].store(PROCEED);
            waitingFlags_.readers[p].store(PROCEED);
        }

        void allow_all_readers_(){
            for (Priority_t i = 0; i < N; ++i){
                waitingFlags_.readers[i].store(PROCEED);
                waitingFlags_.writers[i].store(WAIT);
            }
        }
    };

    #undef WAIT
    #undef PROCEED

}
#endif // BOOST_FAIRNESS_SHARED_PRIORITY_MUTEX_HPP
