
/**
 * @file pause_ops_windows.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the pause operations used by the mutexes on windows.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PAUSE_OPS_WINDOWS_HPP
#define BOOST_FAIRNESS_PAUSE_OPS_WINDOWS_HPP


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN


namespace boost::fairness::detail{

    inline void pause_(){
        YieldProcessor();
    }

}

#endif // BOOST_FAIRNESS_PAUSE_OPS_WINDOWS_HPP