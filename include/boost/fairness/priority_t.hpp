/**
 * @file priority_t.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief Alias the type Priority_t. Priority_t is the type of priorities that are used by the priority_mutexes.
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PRIORITY_T_HPP
#define BOOST_FAIRNESS_PRIORITY_T_HPP
#include <cstdint>
namespace boost::fairness{
    using Priority_t = uint8_t;

    /*
    Priorities are indexes in an array, that means that if I define a priority_mutex<BOOST_FAIRNESS_MAXIMUM_PRIORITY> m;
    we can call m.lock(p) with a p up to BOOST_FAIRNESS_MAXIMUM_PRIORITY - 1;
    this means that a priority to be valid has to be in the range [BOOST_FAIRNESS_MINIMUM_PRIORITY, BOOST_FAIRNESS_MAXIMUM_PRIORITY)
    */
    #define BOOST_FAIRNESS_MAXIMUM_PRIORITY Priority_t(-1)
    #define BOOST_FAIRNESS_MINIMUM_PRIORITY 0

    /*
    structs used to overload unique_lock methods
    */
    struct defer_lock_t { explicit defer_lock_t() = default; };
    struct try_to_lock_t { explicit try_to_lock_t() = default; };
    struct adopt_lock_t { explicit adopt_lock_t() = default; };
    static constexpr defer_lock_t defer_lock;
    static constexpr try_to_lock_t try_to_lock;
    static constexpr adopt_lock_t adopt_lock;
}
#endif // BOOST_FAIRNESS_PRIORITY_T_HPP
