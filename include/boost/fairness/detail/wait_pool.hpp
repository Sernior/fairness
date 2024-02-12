//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file wait_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of a wait pool.
 * @version 0.1
 * @date 2023-12-18
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).

 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

/*
The meaning of this is the following:
The standard lib implementators in all implementations I have seen uses an array of 128 bytes memory regions to avoid false sharing,
then they translate an address (ADDR >> 2 % 16) to an index to the array to determine where to perform the syscall wait.
So given the implementation I say: if we use the std lib implementation then there is no need to 128 byte aligned waiting flags
if the user instead decide to use my customizable wait implementation then we must use waitingFlag which avoid false sharing.
*/

#ifndef BOOST_FAIRNESS_WAIT_POOL_HPP
#define BOOST_FAIRNESS_WAIT_POOL_HPP

#include <atomic>
#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{

    static constexpr uint32_t wait_flag = 0;
    static constexpr uint32_t proceed_flag = 1;

    struct waitingFlag{
        alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) std::atomic<uint32_t> waitMem{};
        //char padding_[BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE - sizeof(waitMem)];

        inline void store(uint32_t const waitStatus){
            waitMem.store(waitStatus);
        }

    };

    template <size_t N>
    class WaitPool {
    public:
        WaitPool() = default;

        std::atomic<uint32_t>& operator[](size_t n) {
#ifdef BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
            return waitMems_[n];
#else
            return waitMems_[n].waitMem;
#endif
        }

        inline void reset_(Priority_t p){
                for (Priority_t i = 0; i < N; ++i)
                    waitMems_[i].store(wait_flag);
                waitMems_[p].store(proceed_flag);
        }

    private:
#ifdef BOOST_FAIRNESS_USE_STD_WAIT_NOTIFY
        std::array<std::atomic<uint32_t>, N> waitMems_{};
#else
        std::array<waitingFlag, N> waitMems_{};
#endif
    };

}
#endif // BOOST_FAIRNESS_WAIT_POOL_HPP
