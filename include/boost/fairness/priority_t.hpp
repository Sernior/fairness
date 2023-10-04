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
    #define BOOST_FAIRNESS_MAXIMUM_PRIORITY Priority_t(-1)
}
#endif // BOOST_FAIRNESS_PRIORITY_T_HPP
