//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file unique_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the unique_lock.
 * @version 0.1
 * @date 2023-10-06
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).
 * 
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 *
 */
#ifndef BOOST_FAIRNESS_UNIQUE_LOCK_HPP
#define BOOST_FAIRNESS_UNIQUE_LOCK_HPP
#include <boost/fairness/priority_t.hpp>
#include <stdexcept>
#include <system_error>
#include <memory>
#include <chrono>

// maybe also all the constructors should check for the validity of the priority

namespace boost::fairness
{
    /**
     * @brief The class unique_lock is a general-purpose mutex ownership wrapper allowing deferred locking, time-constrained attempts at locking, recursive locking, transfer of lock ownership, and use with condition variables.
     * \n 
     * The class unique_lock is movable, but not copyable -- it meets the requirements of [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible) and [MoveAssignable](https://en.cppreference.com/w/cpp/named_req/MoveAssignable) but not of [CopyConstructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) or [CopyAssignable](https://en.cppreference.com/w/cpp/named_req/CopyAssignable).
     * \n 
     * The class unique_lock meets the [BasicLockable](https://en.cppreference.com/w/cpp/named_req/BasicLockable) requirements. If Lockable meets the [Lockable](https://en.cppreference.com/w/cpp/named_req/Lockable) requirements, unique_lock also meets the [Lockable](https://en.cppreference.com/w/cpp/named_req/Lockable) requirements (ex.: can be used in boost::fairness::lock); if Lockable meets the [TimedLockable](https://en.cppreference.com/w/cpp/named_req/TimedLockable) requirements, unique_lock also meets the [TimedLockable](https://en.cppreference.com/w/cpp/named_req/TimedLockable) requirements.
     * 
     * @tparam Lockable : the type of the mutex to lock. The type must meet the [BasicLockable](https://en.cppreference.com/w/cpp/named_req/BasicLockable) requirements.
     */
    template <typename Lockable>
    class unique_lock{

    public:
        /**
         * @brief Construct a new unique lock object
         *
         */
        unique_lock() noexcept
            : lockable_(0), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
        { }

        /**
         * @brief Construct a new unique lock object
         *
         * @param m
         * @param p
         */
        explicit unique_lock(Lockable &m, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
            : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
            if (!is_valid_priority(p))
                throw_operation_not_permitted_();
            lock(p);
            lockOwned_ = true;
            currentPriority_ = p;
        }

        /**
         * @brief Construct a new unique lock object
         *
         * @param m
         */
        unique_lock(Lockable &m, defer_lock_t) noexcept
            : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
        }

        /**
         * @brief Construct a new unique lock object
         *
         * @param m
         * @param p
         */
        unique_lock(Lockable &m, Priority_t const p, try_to_lock_t)
            : lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock(p)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
            if (lockOwned_)
                currentPriority_ = p;
        }

        /**
         * @brief Construct a new unique lock object
         *
         * @param m
         */
        unique_lock(Lockable &m, try_to_lock_t)
            : lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock(BOOST_FAIRNESS_MINIMUM_PRIORITY)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
            if (lockOwned_)
                currentPriority_ = BOOST_FAIRNESS_MINIMUM_PRIORITY;
        }

        /**
         * @brief Construct a new unique lock object
         *
         * @param m
         * @param p
         */
        unique_lock(Lockable &m, Priority_t const p, adopt_lock_t) noexcept
            : lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(p)
        { }

        /*
        There isn`t a non intrusive way to automatically find out the priority 
            - change visibility of current priority, from private to friend
            - currentPriority_ isn't in every mutex.
        */
        /*
        unique_lock(Lockable& m, adopt_lock_t) noexcept
        : lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(m.)
        {
        }*/

        /**
         * @brief Construct a new unique lock object
         *
         * @tparam Clock
         * @tparam Duration
         * @param m
         * @param atime
         * @param p
         */
        template <typename Clock, typename Duration>
        unique_lock(Lockable &m, const std::chrono::time_point<Clock, Duration> &atime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
            : lockable_(std::addressof(m)),
              lockOwned_(lockable_->try_lock_until(atime, p)),
              currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
            if (lockOwned_)
                currentPriority_ = p;
        }

        /**
         * @brief Construct a new unique lock object
         *
         * @tparam Rep
         * @tparam Period
         * @param m
         * @param rtime
         * @param p
         */
        template <typename Rep, typename Period>
        unique_lock(Lockable &m, const std::chrono::duration<Rep, Period> &rtime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
            : lockable_(std::addressof(m)),
              lockOwned_(lockable_->try_lock_for(rtime, p)),
              currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){
            if (lockOwned_)
                currentPriority_ = p;
        }

        /**
         * @brief Destroy the unique lock object
         *
         */
        ~unique_lock(){
            if (lockOwned_)
                unlock();
        }
        unique_lock(const unique_lock &) = delete;
        unique_lock &operator=(const unique_lock &) = delete;

        /**
         * @brief Construct a new unique lock object
         *
         * @param other
         */
        unique_lock(unique_lock &&other) noexcept
            : lockable_(other.lockable_), lockOwned_(other.lockOwned_), currentPriority_(other.currentPriority_){
            other.lockable_ = 0;
            other.lockOwned_ = false;
            other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
        }

        /**
         * @brief Locks the associated mutex. Effectively calls ```mutex()->lock()```.
         *
         * @param p : the priority of the mutex.
		 * 
		 * @return none.
		 * 
		 * @exception
         * 
		 * * Any exceptions thrown by ```mutex()->lock()```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the associated mutex is already locked by this lock (that is, owns_lock() returns true), [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 */
        void lock(Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
            if (!lockable_ || !is_valid_priority(p))
                throw_operation_not_permitted_();
            else if (lockOwned_)
                throw_resource_deadlock_would_occur_();
            else
            {
                lockable_->lock(p);
                lockOwned_ = true;
                currentPriority_ = p;
            }
        }

        /**
         * @brief Tries to lock (i.e., takes ownership of) the associated mutex without blocking. Effectively calls mutex()->try_lock().
		 * \n 
		 * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked.
		 * 
         * @param p : the current priority.
         * 
		 * @return ```true``` if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` otherwise.
         * 
 		 * @exception
         * 
		 * * Any exceptions thrown by ```mutex()->lock()``` ([Mutex](https://en.cppreference.com/w/cpp/named_req/Mutex) types do not throw in try_lock, but a custom [Lockable](https://en.cppreference.com/w/cpp/named_req/Lockable) might);
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the associated mutex is already locked by this unique_lock, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
         */
        bool try_lock(Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
            if (!lockable_ || !is_valid_priority(p))
                throw_operation_not_permitted_();
            else if (lockOwned_)
                throw_resource_deadlock_would_occur_();
            else
            {
                lockOwned_ = lockable_->try_lock(p);
                if (lockOwned_)
                    currentPriority_ = p;
                return lockOwned_;
            }
        }

        /**
         * @brief Unlocks (i.e., releases ownership of) the associated mutex and releases ownership.
         * \n 
         * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is not locked.
         *
		 * @param none.
		 * @return none.
		 * @exception 
		 * 
		 * * Any exceptions thrown by ```mutex()->unlock()```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */
        void unlock(){
            if (!lockOwned_)
                throw_operation_not_permitted_();
            else if (lockable_)
            {
                lockable_->unlock();
                currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
                lockOwned_ = false;
            }
        }

        /**
         * @brief Tries to lock (i.e., takes ownership of) the associated mutex. Blocks until specified atime has been reached or the lock is acquired, whichever comes first. On successful lock acquisition returns ```true```, otherwise returns ```false```. May block for longer than atime until has been reached.
         * \n 
         * Effectively calls ```mutex()->try_lock_until(atime)```.
         * \n 
         * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked by the same thread.
         * \n 
         * Clock must meet the [Clock](https://en.cppreference.com/w/cpp/named_req/Clock) requirements. The program is ill-formed if [std::chrono::is_clock_v](http://en.cppreference.com/w/cpp/chrono/is_clock)<Clock> is ```false```.
         * 
         * @tparam Clock TODO
         * @tparam Duration TODO
         * @param atime : maximum time point to block until
         * @param p : the current priority.
		 * @return ```true``` : if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` : otherwise.
		 * @exception
		 * 
		 * * Any exceptions thrown by ```mutex()->try_lock_until(atime)```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the mutex is already locked, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */        
        template <typename Clock, typename Duration>
        bool try_lock_until(const std::chrono::time_point<Clock, Duration> &atime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
            if (!lockable_ || !is_valid_priority(p))
                throw_operation_not_permitted_();
            else if (lockOwned_)
                throw_resource_deadlock_would_occur_();
            else
            {
                lockOwned_ = lockable_->try_lock_until(atime, p);
                if (lockOwned_)
                    currentPriority_ = p;
                return lockOwned_;
            }
        }

        /**
         * @brief Tries to lock (i.e., takes ownership of) the associated mutex. Blocks until specified rtime has elapsed or the lock is acquired, whichever comes first. On successful lock acquisition returns ```true```, otherwise returns ```false```.Effectively calls ```mutex()->try_lock_for(rtime)```.
         * \n 
         * This function may block for longer than rtime due to scheduling or resource contention delays.
         * \n 
         * The standard recommends that a steady clock is used to measure the duration. If an implementation uses a system clock instead, the wait time may also be sensitive to clock adjustments.
         * \n 
         * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked by this std::unique_lock.
         * 
         * @tparam Clock TODO
         * @tparam Duration TODO
         * @param rtime : maximum duration to block for
         * @param p : the current priority.
		 * @return ```true``` : if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` : otherwise.
		 * @exception
		 * 
		 * * Any exceptions thrown by ```mutex()->try_lock_for(rtime)```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the mutex is already locked, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */        
        template <typename Rep, typename Period>
        bool try_lock_for(const std::chrono::duration<Rep, Period> &rtime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
            if (!lockable_ || !is_valid_priority(p))
                throw_operation_not_permitted_();
            else if (lockOwned_)
                throw_resource_deadlock_would_occur_();
            else
            {
                lockOwned_ = lockable_->try_lock_for(rtime);
                if (lockOwned_)
                    currentPriority_ = p;
                return lockOwned_;
            }
        }

        /**
         * @brief
         *
         * @param other
         * @return unique_lock&
         */
        unique_lock &operator=(unique_lock &&other) noexcept{
            if (lockOwned_)
                unlock();
            unique_lock(std::move(other)).swap(*this);
            other.lockable_ = 0;
            other.lockOwned_ = false;
            other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
            return *this;
        }

        /**
         * @brief Exchanges the internal states of the lock objects.
         *
         * @param other : the lock to swap the state with.
         * @return none.
         */
        void swap(unique_lock &other) noexcept{
            std::swap(lockable_, other.lockable_);
            std::swap(lockOwned_, other.lockOwned_);
            std::swap(currentPriority_, other.currentPriority_);
        }

        /**
         * @brief Breaks the association of the associated mutex, if any, and ```*this```.
         * \n 
         * No locks are unlocked. If ```*this``` held ownership of the associated mutex prior to the call, the caller is now responsible to unlock the mutex.
         * @param none.
         *
         * @return Lockable* : pointer to the associated mutex or a null pointer if there was no associated mutex.
         */
        Lockable *release() noexcept{
            Lockable *__ret = lockable_;
            lockable_ = 0;
            lockOwned_ = false;
            currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
            return __ret;
        }

        /**
         * @brief Checks whether ```*this``` owns a locked mutex or not.
         * @param none.
         * @return true : if ```*this``` has an associated mutex and has acquired ownership of it.
         * @return false : otherwise.
         */
        bool owns_lock() const noexcept
        { return lockOwned_; }

        /**
         * @brief TODO
         *
         * @return Priority_t
         */
        Priority_t lock_priority() const noexcept
        { return currentPriority_; }

        /**
         * @brief Checks whether ```*this``` owns a locked mutex or not. Effectively calls owns_lock().
         * @param none.
         * @return true : if ```*this``` has an associated mutex and has acquired ownership of it.
         * @return false : otherwise.
         */
        explicit operator bool() const noexcept
        { return owns_lock(); }

        /**
         * @brief TODO
         *
         * @return Priority_t
         */
        explicit operator Priority_t() const noexcept
        { return lock_priority(); }

        /**
         * @brief
         *
         * @return Lockable*
         */
        Lockable *mutex() const noexcept
        { return lockable_; }

    private:
        /**
         * @brief
         *
         */
        static inline void throw_operation_not_permitted_()
        { throw std::system_error(std::make_error_code(std::errc::operation_not_permitted)); }

        /**
         * @brief
         *
         */
        static inline void throw_resource_deadlock_would_occur_()
        { throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur)); }

        Priority_t currentPriority_;
        Lockable *lockable_;
        bool lockOwned_;
    };

    /**
     * @brief
     *
     * @tparam Lockable
     * @param lhs
     * @param rhs
     */
    template <typename Lockable>
    inline void swap(unique_lock<Lockable> &lhs, unique_lock<Lockable> &rhs) noexcept
    { lhs.swap(rhs); }

}

#endif // BOOST_FAIRNESS_UNIQUE_LOCK_HPP