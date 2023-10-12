
/**
 * @file wait_ops_generic.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the wait operation for misc os.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
#define BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
#include <boost/fairness/config.hpp>
#include <atomic>

namespace boost::fairness::detail{

    template<typename T>
    inline void wait(T& mem, T expected){
        static_assert(
            std::atomic<T>::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::wait"
        );
        std::atomic<T>(mem).wait(expected, std::memory_order_relaxed);
    }

    template<typename T>
    inline void notify_one(T& mem){
        static_assert(
            std::atomic<T>::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::notify_one"
        );
        std::atomic<T>(mem).notify_one();
    }

    template<typename T>
    inline void notify_all(T& mem){
        static_assert(
            std::atomic<T>::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::notify_one"
        );
        std::atomic<T>(mem).notify_all();
    }

}

#endif // BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
