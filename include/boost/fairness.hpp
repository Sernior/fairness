
/**
 * @file fairness.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the all the includes required by the library.
 * @version 0.1
 * @date 2023-09-29
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_HPP
#define BOOST_FAIRNESS_HPP

#if __cplusplus >= 202002L // boost fairness requires std::atomic::wait/notify

#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/acquisition_modes.hpp>
#include <boost/fairness/priority_mutex.hpp>
#include <boost/fairness/shared_priority_mutex.hpp>
#include <boost/fairness/slim_priority_mutex.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>
#include <boost/fairness/recursive_priority_mutex.hpp>
#include <boost/fairness/unique_lock.hpp>

#endif // __cplusplus >= 202002L

#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
// include experimental stuff here.
#endif // BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES

#endif // BOOST_FAIRNESS_HPP
