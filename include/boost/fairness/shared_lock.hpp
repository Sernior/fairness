
/**
 * @file shared.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the shared.
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

  template<typename Lockable>
  class shared_lock{

    shared_lock() noexcept : lockable_(nullptr), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { }

    explicit shared_lock(Lockable& m, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
      : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { 
        if (!is_valid_priority(p))
            throw_operation_not_permitted_();
        m.lock_shared();
        lockOwned_ = true;
        currentPriority_ = p;
    }

    shared_lock(Lockable& m, defer_lock_t) noexcept
      : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { }

    shared_lock(Lockable& m, try_to_lock_t)
      : lockable_(std::addressof(m)), lockOwned_(m.try_lock_shared(BOOST_FAIRNESS_MINIMUM_PRIORITY)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { 
        if (lockOwned_)
            currentPriority_ = BOOST_FAIRNESS_MINIMUM_PRIORITY;
    }

    shared_lock(Lockable& m, Priority_t const p, try_to_lock_t)
    : lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock_shared(p)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { 
    if (lockOwned_)
        currentPriority_ = p;
    }

    shared_lock(Lockable& m, Priority_t const p, adopt_lock_t) noexcept
    : lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(p)
    {
    }

    template<typename Clock, typename Duration>
	shared_lock(Lockable& m, const std::chrono::time_point<Clock, Duration>& atime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	: lockable_(std::addressof(m)),
	lockOwned_(lockable_->try_lock_shared_until(atime, p)),
    currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
	{ 
      if (lockOwned_)
        currentPriority_ = p;
    }

    template<typename Rep, typename Period>
	shared_lock(Lockable& m, const std::chrono::duration<Rep, Period>& rtime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	: lockable_(std::addressof(m)),
	lockOwned_(lockable_->try_lock_shared_for(rtime, p)),
    currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
	{ 
      if (lockOwned_)
        currentPriority_ = p;
    }

    ~shared_lock()
    {
    if (lockOwned_)
        lockable_->unlock_shared();
    }
    
    shared_lock(const shared_lock&) = delete;
    shared_lock& operator=(const shared_lock&) = delete;

    shared_lock(shared_lock&& other) noexcept
    : lockable_(other.lockable_), lockOwned_(other.lockOwned_), currentPriority_(other.currentPriority_)
    {
      other.lockable_ = 0;
      other.lockOwned_ = false;
      other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    }

    shared_lock& operator=(shared_lock&& other) noexcept
    {
    if(lockOwned_)
      unlock();
    shared_lock(std::move(other)).swap(*this);
    other.lockable_ = 0;
    other.lockOwned_ = false;
    other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    return *this;
    }

    void swap(shared_lock& other) noexcept
    {
    std::swap(lockable_, other.lockable_);
    std::swap(lockOwned_, other.lockOwned_);
    std::swap(currentPriority_, other.currentPriority_);
    }

    Lockable* release() noexcept
    {
    Lockable* __ret = lockable_;
    lockable_ = 0;
    lockOwned_ = false;
    currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    return __ret;
    }

    bool owns_lock() const noexcept
    { return lockOwned_; }

    Priority_t lock_priority()const noexcept
    { return currentPriority_; }

    explicit operator bool() const noexcept
    { return owns_lock(); }

    explicit operator Priority_t() const noexcept
    { return lock_priority(); }

    Lockable* mutex() const noexcept
    { return lockable_; }

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

    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& atime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  {
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

    template<typename Rep, typename Period>
	  bool try_lock_for(const std::chrono::duration<Rep, Period>& rtime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  {
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

    static inline void throw_operation_not_permitted_(){
        throw std::system_error(std::make_error_code(std::errc::operation_not_permitted));
    }
    
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
