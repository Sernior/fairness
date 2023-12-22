/**
 * @file spinlock_priority_mutex.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the spinlock_priority_mutex based on a scalable list base algorithm.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP

#ifdef BOOST_FAIRNESS_USE_TATAS_SPINLOCK
#include <boost/fairness/spinlock_priority_mutex_tatas.hpp>
#endif // BOOST_FAIRNESS_USE_TATAS_SPINLOCK

#ifndef BOOST_FAIRNESS_USE_TATAS_SPINLOCK
#include <boost/fairness/spinlock_priority_mutex_cpl.hpp>
#endif // BOOST_FAIRNESS_USE_TATAS_SPINLOCK

#endif // BOOST_FAIRNESS_SPINLOCK_PRIORITY_MUTEX_HPP
