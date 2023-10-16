/**
 * @file unique_lock.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the unique_lock.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
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

namespace boost::fairness{

  /*
  structs used to overload unique_lock methods
  */
  struct defer_lock_t { explicit defer_lock_t() = default; };
  struct try_to_lock_t { explicit try_to_lock_t() = default; };
  struct adopt_lock_t { explicit adopt_lock_t() = default; };
  static constexpr defer_lock_t defer_lock;
  static constexpr try_to_lock_t try_to_lock;
  static constexpr adopt_lock_t adopt_lock;

  template<typename Lockable>
  class unique_lock{

    public:
    
    /**
     * @brief CONSTRUCTOR DOC TODO
     * 
     * @return CONSTRUCTOR 
     */
    */
    unique_lock() noexcept
    : lockable_(0), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { }

    /*
    CONSTRUCTOR DOC TODO
    */
    explicit unique_lock(Lockable& m, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
    : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    {
    if (!is_valid_priority(p))
      throw_operation_not_permitted_();
    lock(p);
    lockOwned_ = true;
    currentPriority_ = p;
    }

    /*
    CONSTRUCTOR DOC TODO
    */
    unique_lock(Lockable& m, defer_lock_t) noexcept
    : lockable_(std::addressof(m)), lockOwned_(false), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { }

    /*
    CONSTRUCTOR DOC TODO
    */
    unique_lock(Lockable& m, Priority_t const p, try_to_lock_t)
    : lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock(p)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { 
    if (lockOwned_)
        currentPriority_ = p;
    }

    /*
    CONSTRUCTOR DOC TODO
    */
    unique_lock(Lockable& m, try_to_lock_t)
    : lockable_(std::addressof(m)), lockOwned_(lockable_->try_lock(BOOST_FAIRNESS_MINIMUM_PRIORITY)), currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
    { 
    if (lockOwned_)
        currentPriority_ = BOOST_FAIRNESS_MINIMUM_PRIORITY;
    }

    /*
    CONSTRUCTOR DOC TODO
    */
    unique_lock(Lockable& m, Priority_t const p, adopt_lock_t) noexcept
    : lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(p)
    {
    }

    /*
    There isn`t a non intrusive way to automatically find out the priority
    */
    /*
    unique_lock(Lockable& m, adopt_lock_t) noexcept
    : lockable_(std::addressof(m)), lockOwned_(true), currentPriority_(m.)
    {
    }*/

    /*
    CONSTRUCTOR DOC TODO
    */
    template<typename Clock, typename Duration>
	  unique_lock(Lockable& m, const std::chrono::time_point<Clock, Duration>& atime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  : lockable_(std::addressof(m)),
	  lockOwned_(lockable_->try_lock_until(atime, p)),
    currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
	  { 
      if (lockOwned_)
        currentPriority_ = p;
    }

    /*
    CONSTRUCTOR DOC TODO
    */
    template<typename Rep, typename Period>
	  unique_lock(Lockable& m, const std::chrono::duration<Rep, Period>& rtime, Priority_t p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  : lockable_(std::addressof(m)),
	  lockOwned_(lockable_->try_lock_for(rtime, p)),
    currentPriority_(BOOST_FAIRNESS_INVALID_PRIORITY)
	  { 
      if (lockOwned_)
        currentPriority_ = p;
    }

    /*
    DESTRUCTOR DOC TODO
    */
    ~unique_lock()
    {
    if (lockOwned_)
        unlock();
    }
    unique_lock(const unique_lock&) = delete;
    unique_lock& operator=(const unique_lock&) = delete;

    /*
    MOVE CONSTRUCTOR DOC TODO
    */
    unique_lock(unique_lock&& other) noexcept
    : lockable_(other.lockable_), lockOwned_(other.lockOwned_), currentPriority_(other.currentPriority_)
    {
      other.lockable_ = 0;
      other.lockOwned_ = false;
        other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    }

    /*
    METHOD LOCK DOC TODO
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

    /*
    METHOD TRY_LOCK DOC TODO
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

    /*
    METHOD UNLOCK DOC TODO
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

    /*
    METHOD TRY_LOCK_UNTIL DOC TODO
    */
    template<typename Clock, typename Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& atime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  {
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

    /*
    METHOD TRY_LOCK_FOR DOC TODO
    */
    template<typename Rep, typename Period>
	  bool try_lock_for(const std::chrono::duration<Rep, Period>& rtime, Priority_t const p = BOOST_FAIRNESS_MINIMUM_PRIORITY)
	  {
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

    /*
    OP ASSIGNMENT DOC TODO
    */
    unique_lock& operator=(unique_lock&& other) noexcept
    {
    if(lockOwned_)
      unlock();
    unique_lock(std::move(other)).swap(*this);
    other.lockable_ = 0;
    other.lockOwned_ = false;
    other.currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    return *this;
    }

    /*
    SWAP METHOD DOC TODO
    */
    void swap(unique_lock& other) noexcept
    {
    std::swap(lockable_, other.lockable_);
    std::swap(lockOwned_, other.lockOwned_);
    std::swap(currentPriority_, other.currentPriority_);
    }

    /*
    RELEASE METHOD DOC TODO
    */
    Lockable* release() noexcept
    {
    Lockable* __ret = lockable_;
    lockable_ = 0;
    lockOwned_ = false;
    currentPriority_ = BOOST_FAIRNESS_INVALID_PRIORITY;
    return __ret;
    }

    /*
    OWNS LOCK METHOD DOC TODO
    */
    bool owns_lock() const noexcept
    { return lockOwned_; }

    /*
    LOCK PRIORITY METHOD DOC TODO
    */
    Priority_t lock_priority()const noexcept
    { return currentPriority_; }

    /*
    BOOL CAST OP DOC TODO
    */
    explicit operator bool() const noexcept
    { return owns_lock(); }

    /*
    PRIORITY_T CAST OP DOC TODO
    */
    explicit operator Priority_t() const noexcept
    { return lock_priority(); }

    /*
    MUTEX METHOD DOC TODO
    */
    Lockable* mutex() const noexcept
    { return lockable_; }

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

  /*
  SWAP FUNCTION DOC TODO
  */
  template<typename Lockable>
  inline void swap(unique_lock<Lockable>& lhs, unique_lock<Lockable>& rhs) noexcept
  { lhs.swap(rhs); }

}

#endif // BOOST_FAIRNESS_UNIQUE_LOCK_HPP