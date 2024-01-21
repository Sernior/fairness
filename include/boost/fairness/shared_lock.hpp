
/**
 * @file shared.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the shared_lock.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SHARED_LOCK_HPP
#define BOOST_FAIRNESS_SHARED_LOCK_HPP
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/acquisition_modes.hpp>
#include <stdexcept>
#include <system_error>
#include <memory>
#include <chrono>

// maybe also all the constructors should check for the validity of the priority

namespace boost::fairness{

	/**
	 * @brief The class shared_lock is a general-purpose shared mutex ownership wrapper allowing deferred locking, timed locking and transfer of lock ownership. Locking a shared_lock locks the associated shared mutex in shared mode (to lock it in exclusive mode, [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock) can be used).
	 * \n 
	 * The shared_lock class is movable, but not copyable – it meets the requirements of [MoveConstructible](https://en.cppreference.com/w/cpp/named_req/MoveConstructible) and [MoveAssignable](https://en.cppreference.com/w/cpp/named_req/MoveAssignable) but not of [CopyConstructible](https://en.cppreference.com/w/cpp/named_req/CopyConstructible) or [CopyAssignable](https://en.cppreference.com/w/cpp/named_req/CopyAssignable).
	 * \n 
	 * shared_lock meets the [Lockable](https://en.cppreference.com/w/cpp/named_req/Lockable) requirements. If Lockable meets the [SharedTimedLockable](https://en.cppreference.com/w/cpp/named_req/SharedTimedLockable) requirements, shared_lock also meets [TimedLockable](https://en.cppreference.com/w/cpp/named_req/TimedLockable) requirements.
	 * \n 
	 * In order to wait in a shared mutex in shared ownership mode, [std::condition_variable_any](https://en.cppreference.com/w/cpp/thread/condition_variable_any) can be used ([std::condition_variable](https://en.cppreference.com/w/cpp/thread/condition_variable) requires [std::unique_lock](https://en.cppreference.com/w/cpp/thread/unique_lock) and so can only wait in unique ownership mode).
	 * @tparam Lockable : the type of the shared mutex to lock. The type must meet the [SharedLockable](https://en.cppreference.com/w/cpp/named_req/SharedLockable) requirements.
	 */
	template<typename Lockable>
	class shared_lock{
		
		public:

		/**
		 * @brief Construct a new shared lock object
		 * @private
		 */
		shared_lock() noexcept : lockable_(nullptr), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
		{ }
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param m 
		 * @param p 
		 * @private
		 */
		explicit shared_lock(Lockable& m, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
		: lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){ 
			if (!is_valid_priority(p))
				throw_operation_not_permitted_();
			m.lock_shared();
			lockOwned_ = true;
			currentPriority_ = p;
		}
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param m 
		 * @private
		 */
		shared_lock(Lockable& m, defer_lock_t) noexcept
		: lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
		{ }
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param m 
		 * @private
		 */
		shared_lock(Lockable& m, try_to_lock_t)
		: lockable_(std::addressof(m)), lockOwned_(m.try_lock_shared(BOOST_FAIRNESS_MINIMUM_PRIORITY)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){ 
			if (lockOwned_)
				currentPriority_ = BOOST_FAIRNESS_MINIMUM_PRIORITY;
		}
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param m 
		 * @param p 
		 * @private
		 */
		shared_lock(Lockable& m, Priority_t const p, try_to_lock_t)
		: lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock_shared(p)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){ 
			if (lockOwned_)
				currentPriority_ = p;
		}
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param m 
		 * @param p 
		 * @private
		 */
		shared_lock(Lockable& m, Priority_t const p, adopt_lock_t) noexcept
		: lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(p)
		{ }
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @tparam Clock 
		 * @tparam Duration 
		 * @param m 
		 * @param atime 
		 * @param p 
		 * @private
		 */
		template<typename Clock, typename Duration>
		shared_lock(Lockable& m, const std::chrono::time_point<Clock, Duration>& atime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
		: lockable_(std::addressof(m)),
		lockOwned_(lockable_->try_lock_shared_until(atime, p)),
		currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){ 
			if (lockOwned_)
				currentPriority_ = p;
		}
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @tparam Rep 
		 * @tparam Period 
		 * @param m 
		 * @param rtime 
		 * @param p 
		 * @private
		 */
		template<typename Rep, typename Period>
		shared_lock(Lockable& m, const std::chrono::duration<Rep, Period>& rtime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
		: lockable_(std::addressof(m)),
		lockOwned_(lockable_->try_lock_shared_for(rtime, p)),
		currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY){ 
			if (lockOwned_)
				currentPriority_ = p;
		}
		
		/**
		 * @brief Destroy the shared lock object
		 * @private
		 */
		~shared_lock(){
			if (lockOwned_)
				lockable_->unlock_shared();
		}
		
		/**
		 * @brief Construct a new shared lock object
		 * @private
		 */
		shared_lock(const shared_lock&) = delete;
		
		/**
		 * @brief 
		 * 
		 * @return shared_lock& 
		 * @private
		 */
		shared_lock& operator=(const shared_lock&) = delete;
		
		/**
		 * @brief Construct a new shared lock object
		 * 
		 * @param other 
		 * @private
		 */
		shared_lock(shared_lock&& other) noexcept
		: lockable_(other.lockable_), lockOwned_(other.lockOwned_), currentPriority_(other.currentPriority_){
			other.lockable_ = 0;
			other.lockOwned_ = false;
			other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
		}
		
		/**
		 * @brief 
		 * 
		 * @param other 
		 * @return shared_lock& 
		 * @private
		 */
		shared_lock& operator=(shared_lock&& other) noexcept{
			if(lockOwned_)
				unlock();
			shared_lock(std::move(other)).swap(*this);
			other.lockable_ = 0;
			other.lockOwned_ = false;
			other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
			return *this;
		}
		
		/**
		 * @brief Exchanges the internal states of the lock objects.
		 * 
		 * @param other : the lock to swap the state with.
		 */
		void swap(shared_lock& other) noexcept{
			std::swap(lockable_, other.lockable_);
			std::swap(lockOwned_, other.lockOwned_);
			std::swap(currentPriority_, other.currentPriority_);
		}
		
		/**
		 * @brief Breaks the association of the associated mutex, if any, and ```*this```.
		 * \n
		 * No locks are unlocked. If the ```*this``` held ownership of the associated mutex prior to the call, the caller is now responsible to unlock the mutex.
		 * 
		 * @return Lockable* : Pointer to the associated mutex or a null pointer if there was no associated mutex.
		 */
		Lockable* release() noexcept{
			Lockable* __ret = lockable_;
			lockable_ = 0;
			lockOwned_ = false;
			currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
			return __ret;
		}
		
		/**
		 * @brief Checks whether ```*this``` owns a locked mutex or not.
		 * 
		 * @return ```true``` if *this has an associated mutex and has acquired shared ownership of it.
		 * @return ```false``` otherwise. 
		 * 
		 */
		bool owns_lock() const noexcept
		{ return lockOwned_; }
		
		/**
		 * @brief Checks the current priority of ```*this```. #TODO
		 * 
		 * @return Priority_t : the current priority.
		 */
		Priority_t lock_priority()const noexcept
		{ return currentPriority_; }

		/**
		 * @brief Checks whether ```*this owns``` a locked mutex or not. Effectively calls owns_lock().
		 * 
		 * @return ```true``` if *this has an associated mutex and has acquired shared ownership of it.
		 * @return ```false``` otherwise. 
		 * 
		 */
		explicit operator bool() const noexcept
		{ return owns_lock(); }

		/**
		 * @brief 
		 * 
		 * @return Priority_t 
		 * @private
		 */
		explicit operator Priority_t() const noexcept
		{ return lock_priority(); }

		/**
		 * @brief Returns a pointer to the associated mutex, or a null pointer if there is no associated mutex.
		 * 
		 * @return Lockable* : Pointer to the associated mutex or a null pointer if there is no associated mutex.
		 */
		Lockable* mutex() const noexcept
		{ return lockable_; }

		/**
		 * @brief Locks the associated mutex in shared mode. Effectively calls ```mutex()->lock_shared()```.
		 * 
		 * @param p : the priority of the mutex.
		 * 
		 * @return none.
		 * 
		 * @exception
		 * 
		 * * Any exceptions thrown by ```mutex()->lock_shared()```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the associated mutex is already locked by this shared_lock (that is, owns_lock() returns true), [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 */
		void lock(Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
			if (!lockable_ || !is_valid_priority(p))
				throw_operation_not_permitted_();
			else if (lockOwned_)
				throw_resource_deadlock_would_occur_();
			else
			{
				lockable_->lock_shared(p);
				lockOwned_ = true;
				currentPriority_ = p;
			}
		}

		/**
		 * @brief Tries to lock the associated mutex in shared mode without blocking. Effectively calls ```mutex()->try_lock_shared()```.
		 * \n 
		 * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked.
		 * 
		 * @param p : the current priority.
		 * @return ```true``` if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` otherwise.
		 */
		bool try_lock(Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
			if (!lockable_ || !is_valid_priority(p))
				throw_operation_not_permitted_();
			else if (lockOwned_)
				throw_resource_deadlock_would_occur_();
			else
			{
				lockOwned_ = lockable_->try_lock_shared(p);
				if (lockOwned_)
					currentPriority_ = p;
				return lockOwned_;
			}
		}

		/**
		 * @brief Unlocks the associated mutex from shared mode. Effectively calls ```mutex()->unlock_shared()```.
		 * \n 
		 * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is not locked.
		 * @param none.
		 * @return none.
		 * @exception 
		 * 
		 * * Any exceptions thrown by ```mutex()->unlock_shared()```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */
		void unlock(){
			if (!lockOwned_)
				throw_operation_not_permitted_();
			else if (lockable_)
			{
				lockable_->unlock_shared();
				currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
				lockOwned_ = false;
			}
		}
		/**
		 * @brief Tries to lock the associated mutex in shared mode. Blocks until specified ```atime``` has been reached or the lock is acquired, whichever comes first. On successful lock acquisition returns ```true```, otherwise returns ```false```. May block for longer than until ```atime``` has been reached.
		 * \n 
		 * Effectively calls ```mutex()->try_lock_shared_until(timeout_time)```.
		 * \n 
		 * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked.
		 * \n
		 * Clock must meet the [Clock](https://en.cppreference.com/w/cpp/named_req/Clock) requirements. The behavior is undefined if Mutex does not meet the [SharedTimedLockable](https://en.cppreference.com/w/cpp/named_req/SharedTimedLockable) requirements. The program is ill-formed if [std::chrono::is_clock_v](http://en.cppreference.com/w/cpp/chrono/is_clock)<Clock> is ```false```.
		 * 
		 * @tparam Clock TODO
		 * @tparam Duration TODO
		 * @param atime : 	maximum time point to block until.
		 * @param p : the current priority.
		 * @return ```true``` : if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` : otherwise.
		 * @exception
		 * 
		 * * Any exceptions thrown by ```mutex()->try_lock_shared_until(atime)```;
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the mutex is already locked, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */
		template<typename Clock, typename Duration>
		bool try_lock_until(const std::chrono::time_point<Clock, Duration>& atime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
			if (!lockable_ || !is_valid_priority(p))
				throw_operation_not_permitted_();
			else if (lockOwned_)
				throw_resource_deadlock_would_occur_();
			else
			{
				lockOwned_ = lockable_->try_lock_shared_until(atime, p);
				if (lockOwned_)
				currentPriority_ = p;
				return lockOwned_;
			}
		}

		/**
		 * @brief Tries to lock the associated mutex in shared mode. Blocks until specified rtime has elapsed or the lock is acquired, whichever comes first. On successful lock acquisition returns ```true```, otherwise returns ```false```. Effectively calls ```mutex()->try_lock_shared_for(rtime)```.
		 * \n 
		 * This function may block for longer than rtime due to scheduling or resource contention delays.
		 * \n 
		 * The standard recommends that a steady clock is used to measure the duration. If an implementation uses a system clock instead, the wait time may also be sensitive to clock adjustments.
		 * \n 
		 * [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) is thrown if there is no associated mutex or if the mutex is already locked.
		 * \n 
		 * The behavior is undefined if Lockable does not meet the [SharedTimedLockable](https://en.cppreference.com/w/cpp/named_req/SharedTimedLockable) requirements.
		 * 
		 * @tparam Rep TODO
		 * @tparam Period TODO
		 * @param rtime : maximum duration to block for.
		 * @param p 
		 * @return ```true``` : if the ownership of the mutex has been acquired successfully.
		 * @return ```false``` : otherwise.
		 * @exception
		 * 
		 * * Any exceptions thrown by ```mutex()->try_lock_shared_for(timeout_duration)```.
		 * * If there is no associated mutex, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::operation_not_permitted](https://en.cppreference.com/w/cpp/error/errc);
		 * * If the mutex is already locked, [std::system_error](https://en.cppreference.com/w/cpp/error/system_error) with an error code of [std::errc::resource_deadlock_would_occur](https://en.cppreference.com/w/cpp/error/errc).
		 * 
		 */
		template<typename Rep, typename Period>
		bool try_lock_for(const std::chrono::duration<Rep, Period>& rtime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY){
			if (!lockable_ || !is_valid_priority(p))
				throw_operation_not_permitted_();
			else if (lockOwned_)
				throw_resource_deadlock_would_occur_();
			else
			{
				lockOwned_ = lockable_->try_lock_shared_for(rtime, p);
				if (lockOwned_)
					currentPriority_ = p;
				return lockOwned_;
			}
		}

		private:

		/**
		 * @brief 
		 * @private
		 */
		static inline void throw_operation_not_permitted_(){
			throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
		}
		
		/**
		 * @brief 
		 * @private
		 */
		static inline void throw_resource_deadlock_would_occur_(){
			throw std::system_error(std::make_error_code(std::errc::resource_deadlock_would_occur));
		}

		Priority_t  currentPriority_;
		Lockable*	lockable_;
		bool		lockOwned_;

		};

	template<typename Lockable>
	inline void swap(shared_lock<Lockable>& lhs, shared_lock<Lockable>& rhs) noexcept
	{ lhs.swap(rhs); }

}

#endif // BOOST_FAIRNESS_SHARED_LOCK_HPP
