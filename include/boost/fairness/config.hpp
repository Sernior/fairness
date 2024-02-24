/**
 * @file config.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains configurations about the library.
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_CONFIG_HPP
#define BOOST_FAIRNESS_CONFIG_HPP



#ifdef BOOST_FAIRNESS_USE_TATAS
/**
 * @brief Base the mutex implementation on simple non scalable but very fast TATAS. Only use on machines with less than 4 physical cores.
 */
#define BOOST_FAIRNESS_USE_TATAS_SPINLOCK
#endif // BOOST_FAIRNESS_USE_TATAS



#ifndef BOOST_FAIRNESS_GETREQUEST_SPINS
/**
 * @brief The total number of spins performed while performing a getRequest (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined) (unused).
 */
#define BOOST_FAIRNESS_GETREQUEST_SPINS 1
#endif // BOOST_FAIRNESS_GETREQUEST_SPINS



#ifndef BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED
/**
 * @brief The number of paused spins performed while performing a getRequest (this is used only if BOOST_FAIRNESS_USE_TATAS is not defined) (unused).
 */
#define BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED 1
#endif // BOOST_FAIRNESS_GETREQUEST_SPINS_RELAXED



#ifndef BOOST_FAIRNESS_SPINWAIT_SPINS
/**
 * @brief The total number of spins during a spin_wait operation
 */
#define BOOST_FAIRNESS_SPINWAIT_SPINS 16
#endif // BOOST_FAIRNESS_SPINWAIT_SPINS



#ifndef BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED
/**
 * @brief The number of relaxed spins during a spin_wait operation
 */
#define BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED 12
#endif // BOOST_FAIRNESS_SPINWAIT_SPINS_RELAXED



#ifndef BOOST_FAIRNESS_WAIT_SPINS
/**
 * @brief The total number of spins during a wait operation before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows
 */
#define BOOST_FAIRNESS_WAIT_SPINS 16
#endif // BOOST_FAIRNESS_WAIT_SPINS



#ifndef BOOST_FAIRNESS_WAIT_SPINS_RELAXED
/**
 * @brief The total number of spins during a wait operation before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows.
 */
#define BOOST_FAIRNESS_WAIT_SPINS_RELAXED 12
#endif // BOOST_FAIRNESS_WAIT_SPINS_RELAXED



#ifndef BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE
/**
 * @brief Size to be aligned to avoid [false sharing](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html#optimization - 3.7.3)
 */
#define BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE 128
#endif // BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE



#ifndef BOOST_FAIRNESS_MAX_PQNODES
/**
 * @brief The maximum amount of PQNODES that can be used at the same time per pqspinlock.
 * \n
 * Setting this too low may make the mutex less fair (some tests might even fail). Ideally you have 1 PQNODE per thread.
 */
#define BOOST_FAIRNESS_MAX_PQNODES 4
#endif // BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE
#define BOOST_FAIRNESS_INVALID_PQNODE_INDEX BOOST_FAIRNESS_MAX_PQNODES
static_assert(BOOST_FAIRNESS_MAX_PQNODES > 2, "BOOST_FAIRNESS_MAX_PQNODES less than 3.");



#ifndef BOOST_FAIRNESS_MAX_THREADS
/**
 * @brief Unlike what it may seems this macro does not set the maximum number of supported threads.
 * \n
 * Instead this macro tells the library how many thread struct there are per thread/pqspinlock pair to be used.
 * \n
 * A good way of seeing it is how many re entrant calls will you make on priority mutexes.
 * \n
 * If you plan on using N priority mutexes and locking all of them with the same thread set this number to N.
 */
#define BOOST_FAIRNESS_MAX_THREADS 4
#endif // BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE
#define BOOST_FAIRNESS_INVALID_THREAD_INDEX BOOST_FAIRNESS_MAX_THREADS



/*
 If we have boost lib include boost atomic
 */
#ifdef BOOST_FOUND
#include <boost/atomic.hpp>
#endif // BOOST_FOUND



#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0
/**
 * @brief If the hardware supports it and boost::atomic is present then boost::fairness is allowed to use cmpxch16b
 */
#define BOOST_FAIRNESS_HAS_DWCAS

#endif // BOOST_FAIRNESS_HAS_DWCAS



/**
 * @brief boost fairness will use atomic::wait/notify implemented by the standard lib instead of its own implementation by default.
 */
#define BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY




/**
 * @brief If BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY is defined we shall use the experimental wait notify system
 */
#if defined(BOOST_FAIRNESS_USE_EXPERIMENTAL_WAIT_NOTIFY)
#undef BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
#endif



/*
 Operating systems
 */
#if defined(_WIN32)
#include <boost/fairness/detail/pause_ops_windows.hpp>
#else
#include <boost/fairness/detail/pause_ops_linux.hpp>
#endif
#if defined(__linux__) && !defined(BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY)
#include <boost/fairness/detail/wait_ops_linux.hpp>
#elif defined(_WIN32) && !defined(BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY)
#include <boost/fairness/detail/wait_ops_windows.hpp>
#else

#ifndef BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
/**
 * @brief boost::fairness will use atomic::wait/notify implemented by the standard lib instead of its own implementation.
 */
#define BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
#endif // BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY

#include <boost/fairness/detail/wait_ops_generic.hpp>
#endif // Operating systems



#endif // BOOST_FAIRNESS_CONFIG_HPP
