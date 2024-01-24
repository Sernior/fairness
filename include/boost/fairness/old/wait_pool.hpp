/**
 * @file wait_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of a wait pool.
 * @version 0.1
 * @date 2023-12-18
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

// UNUSED YET ... but this should be the implementation of the memory regions we are supposed to do the wait / notify

#ifndef BOOST_FAIRNESS_WAIT_POOL_HPP
#define BOOST_FAIRNESS_WAIT_POOL_HPP

#include <atomic>
#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{

    struct waitingFlag{
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) uint32_t waitMem{};
    };

    template<size_t N>
    class WaitPool{
    public:

        WaitPool() = default;



    private:

        std::array<waitingFlag, N> waitMems_{};
    };

    static WaitPool<16> waitPool_;

}
#endif // BOOST_FAIRNESS_WAIT_POOL_HPP
