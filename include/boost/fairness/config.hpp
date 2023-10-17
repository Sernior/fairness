/**
 * @file config.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains configurations about boost and 128bit cpu support. TODO
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_CONFIG_HPP
#define BOOST_FAIRNESS_CONFIG_HPP
#ifdef BOOST_FOUND
#include <boost/atomic.hpp>
#endif // BOOST_FOUND
#if defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0
#define BOOST_FAIRNESS_HAS_DWCAS
#endif // defined(BOOST_ATOMIC_INT128_LOCK_FREE) && BOOST_ATOMIC_INT128_LOCK_FREE > 0
#endif // BOOST_FAIRNESS_CONFIG_HPP
