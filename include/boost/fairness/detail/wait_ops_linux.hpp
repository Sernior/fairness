
/**
 * @file wait_ops_linux.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the wait operation specific for linux.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_WAIT_OPS_LINUX_HPP
#define BOOST_FAIRNESS_WAIT_OPS_LINUX_HPP
#include <boost/fairness/config.hpp>
#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <limits.h>

namespace boost::fairness::detail{

    template<typename T, typename K>
    inline void wait_(T& mem, K expected){
        static_assert(sizeof(T) == 4, "Invalid argument size on boost::fairness::detail::wait");
        syscall(SYS_futex, &mem, FUTEX_WAIT_PRIVATE, expected, nullptr, nullptr, 0);
    }

    template<typename T>
    inline void notify_one_(T& mem){
        static_assert(sizeof(T) == 4, "Invalid argument size on boost::fairness::detail::notify_one");
        syscall(SYS_futex, &mem, FUTEX_WAKE_PRIVATE, 1, nullptr, nullptr, 0);
    }

    template<typename T>
    inline void notify_all_(T& mem){
        static_assert(sizeof(T) == 4, "Invalid argument size on boost::fairness::detail::notify_all");
        syscall(SYS_futex, &mem, FUTEX_WAKE_PRIVATE, INT_MAX, nullptr, nullptr, 0);
    }

}

#endif // BOOST_FAIRNESS_WAIT_OPS_LINUX_HPP
