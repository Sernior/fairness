/**
 * @file thread_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a pool of thread structs.
 * @version 0.1
 * @date 2023-12-18
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_THREAD_POOL_HPP
#define BOOST_FAIRNESS_THREAD_POOL_HPP

#include <atomic>
#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{

    template<size_t N>
    class ThreadPool{
    public:

        ThreadPool() = default;



    private:

        std::array<Thread, N> waitMems_;
    };

    static ThreadPool<16> waitPool_;

}
#endif // BOOST_FAIRNESS_THREAD_POOL_HPP
