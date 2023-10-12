/**
 * @file priority_t.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
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
    /**
     * @brief Priorities are indexes in an array, that means that if I define a priority_mutex<BOOST_FAIRNESS_MAXIMUM_PRIORITY> m;
     * we can call m.lock(p) with a p up to BOOST_FAIRNESS_MAXIMUM_PRIORITY - 1;
     * this means that a priority to be valid has to be in the range [BOOST_FAIRNESS_MINIMUM_PRIORITY, BOOST_FAIRNESS_MAXIMUM_PRIORITY)
     * 
     */
    #define BOOST_FAIRNESS_MAXIMUM_PRIORITY Priority_t(-1)
    /**
     * @brief To assign a priority to an invalid state. TODO
     * 
     */
    #define BOOST_FAIRNESS_INVALID_PRIORITY Priority_t(-1)
    /**
     * @brief Priorities are indexes in an array, that means that if I define a priority_mutex<BOOST_FAIRNESS_MAXIMUM_PRIORITY> m;
     * we can call m.lock(p) with a p up to BOOST_FAIRNESS_MAXIMUM_PRIORITY - 1;
     * this means that a priority to be valid has to be in the range [BOOST_FAIRNESS_MINIMUM_PRIORITY, BOOST_FAIRNESS_MAXIMUM_PRIORITY)
     * 
     */
    #define BOOST_FAIRNESS_MINIMUM_PRIORITY 0

    /**
     * @brief allows you to verify that an input priority is valid. Has to be in the range [BOOST_FAIRNESS_MINIMUM_PRIORITY, BOOST_FAIRNESS_MAXIMUM_PRIORITY).
     * 
     * @param p : The priority to verify.
     * @return true : if is valid.
     * @return false : if isn't valid.
     */
    inline bool is_valid_priority(Priority_t const p){
        return p >= BOOST_FAIRNESS_MINIMUM_PRIORITY && p < BOOST_FAIRNESS_MAXIMUM_PRIORITY;
    }

} // namespace boost::fairness
#endif // BOOST_FAIRNESS_PRIORITY_T_HPP
