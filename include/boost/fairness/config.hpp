/**
 * @file config.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief This file contains the implementation of the slim_spinlock_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_CONFIG_HPP
#define BOOST_FAIRNESS_CONFIG_HPP



#ifndef BOOST_FAIRNESS_WAIT_SPINS
/**
 * @brief the number of unpaused spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows
*/
#define BOOST_FAIRNESS_WAIT_SPINS 16
#endif // BOOST_FAIRNESS_WAIT_SPINS



#ifndef BOOST_FAIRNESS_WAIT_SPINS_RELAXED
/**
 * @brief the number of paused spins before a syscall to the OS to yield the cpu through futex on linux or waitOnAdress on windows
*/
#define BOOST_FAIRNESS_WAIT_SPINS_RELAXED 64
#endif // BOOST_FAIRNESS_WAIT_SPINS_RELAXED



/*
 If we have boost lib include boost atomic
*/
#ifdef BOOST_FOUND
#include <boost/atomic.hpp>
#endif // BOOST_FOUND



/*
 If we have boost atomic check that our hardware supports DWCAS
*/
#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0
/**
 * @brief if the hardware supports it and boost::atomic is present then boost::fairness is allowed to use cmpxch16b
*/
#define BOOST_FAIRNESS_HAS_DWCAS
#endif // BOOST_FAIRNESS_HAS_DWCAS



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
 * @brief boost fairness will use atomic::wait/notify implemented by the standard lib instead of its own implementation.
*/
#define BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
#endif // BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY

#include <boost/fairness/detail/wait_ops_generic.hpp>
#endif // Operating systems



#endif // BOOST_FAIRNESS_CONFIG_HPP
