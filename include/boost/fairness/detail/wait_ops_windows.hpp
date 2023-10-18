
/**
 * @file wait_ops_windows.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the wait operation specific for windows.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_WAIT_OPS_WINDOWS_HPP
#define BOOST_FAIRNESS_WAIT_OPS_WINDOWS_HPP
#include <boost/fairness/config.hpp>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#undef WIN32_LEAN_AND_MEAN

namespace boost::fairness::detail{

    template<typename T, typename K>
    inline void wait_(T& mem, K expected){
        static_assert(
            sizeof(T) == 1 ||
            sizeof(T) == 2 ||
            sizeof(T) == 4 ||
            sizeof(T) == 8 ||
            , "Invalid argument size on boost::fairness::detail::wait"
        );
        WaitOnAddress(&mem, &expected, sizeof(expected), INFINITE);
    }

    template<typename T>
    inline void notify_one_(T& mem){
        static_assert(
            sizeof(T) == 1 ||
            sizeof(T) == 2 ||
            sizeof(T) == 4 ||
            sizeof(T) == 8 ||
            , "Invalid argument size on boost::fairness::detail::notify_one"
        );
        WakeByAddressSingle(&mem);
    }

    template<typename T>
    inline void notify_all_(T& mem){
        static_assert(
            sizeof(T) == 1 ||
            sizeof(T) == 2 ||
            sizeof(T) == 4 ||
            sizeof(T) == 8 ||
            , "Invalid argument size on boost::fairness::detail::notify_all"
        );
        WakeByAddressAll(&mem);
    }

}

#endif // BOOST_FAIRNESS_WAIT_OPS_WINDOWS_HPP
