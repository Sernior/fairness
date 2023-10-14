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

#ifndef BOOST_FAIRNESS_WAIT_SPINS_FAST
#define BOOST_FAIRNESS_WAIT_SPINS_FAST 16
#endif

#ifdef BOOST_FOUND
#include <boost/atomic.hpp>
#endif // BOOST_FOUND

#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0
#define BOOST_FAIRNESS_HAS_DWCAS
#endif // BOOST_FAIRNESS_HAS_DWCAS

#if defined(__linux__)
#include <boost/fairness/detail/wait_ops_linux.hpp>
#elif defined(_WIN32)
#include <boost/fairness/detail/wait_ops_windows.hpp>
#else
#define BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
#include <boost/fairness/detail/wait_ops_generic.hpp>
#endif //__linux__

#endif // BOOST_FAIRNESS_CONFIG_HPP
