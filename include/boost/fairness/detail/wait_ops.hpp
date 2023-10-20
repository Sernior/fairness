
/**
 * @file wait_ops.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of the wait operations used by the mutexes.
 * @version 0.1
 * @date 2023-10-06
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_WAIT_OPS_HPP
#define BOOST_FAIRNESS_WAIT_OPS_HPP
#include <boost/fairness/config.hpp>
#include <boost/fairness/detail/pause_ops.hpp>
#include <atomic>
#include <thread>


namespace boost::fairness::detail{

#if !defined(BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY)

    template<typename T, typename K>
    inline void wait(T& mem, K expected){
        while (mem.load(std::memory_order_relaxed) == expected){
            for(int i = 0; i < BOOST_FAIRNESS_WAIT_SPINS; ++i){
                if (mem.load(std::memory_order_relaxed) != expected)
                    return;
            }
            for(int i = 0; i < BOOST_FAIRNESS_WAIT_SPINS_RELAXED; ++i){
                if (mem.load(std::memory_order_relaxed) != expected)
                    return;
                pause();
            }
            for(int i = 0; i < BOOST_FAIRNESS_WAIT_SPINS_YIELD; ++i){
                if (mem.load(std::memory_order_relaxed) != expected)
                    return;
                std::this_thread::yield();
            }
            wait_(mem, expected);
        }
    }

#else

    template<typename T, typename K>
    inline void wait(T& mem, K expected){
        wait_(mem, expected);
    }

#endif // BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY

    template<typename T>
    inline void notify_one(T& mem){
        notify_one_(mem);
    }

    template<typename T>
    inline void notify_all(T& mem){
        notify_all_(mem);
    }

}

#endif // BOOST_FAIRNESS_WAIT_OPS_HPP
