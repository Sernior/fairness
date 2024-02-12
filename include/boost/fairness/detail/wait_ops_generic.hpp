//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file wait_ops_generic.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the wait operation for misc os.
 * @version 0.1
 * @date 2023-10-06
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).

 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
#define BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
#include <boost/fairness/config.hpp>
#include <atomic>

namespace boost::fairness::detail{

    template<typename T, typename K>
    inline void wait_(T& mem, K expected){
        static_assert(
            T::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::wait"
        );
        mem.wait(expected, std::memory_order_relaxed);
    }

    template<typename T>
    inline void notify_one_(T& mem){
        static_assert(
            T::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::notify_one"
        );
        mem.notify_one();
    }

    template<typename T>
    inline void notify_all_(T& mem){
        static_assert(
            T::is_always_lock_free
            , "Invalid argument size on boost::fairness::detail::notify_one"
        );
        mem.notify_all();
    }

}

#endif // BOOST_FAIRNESS_WAIT_OPS_GENERIC_HPP
