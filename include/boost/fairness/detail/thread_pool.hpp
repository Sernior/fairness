//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file thread_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of a pool of thread structs.
 * @version 0.1
 * @date 2023-12-18
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).

 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_THREAD_POOL_HPP
#define BOOST_FAIRNESS_THREAD_POOL_HPP

#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{

    class pqspinlock;

    struct Thread{

        Thread() = default;

        void prepare(Priority_t p, Request* req){

            request_ = req;

            req->thread_ = this;

            priority_ = p;

            watch_ = nullptr;
        }

        Request* watch_{nullptr};
        Request* request_{nullptr};
        void* owner_{nullptr};
        Priority_t priority_{BOOST_FAIRNESS_INVALID_PRIORITY};
        bool inUse_{};
    }; 

    template<size_t N>
    class ThreadPool{
    public:

        ThreadPool() = default;

        Thread* getThread(void* const owner){
            for (uint32_t i = 0; i < N; ++i){
                if (!threads_[i].inUse_){
                    threads_[i].owner_ = owner;
                    return &threads_[i];
                }
            }
            return nullptr;
        }

        Thread* reGetThread(void* const owner){
            for (uint32_t i = 0; i < N; ++i){
                if (threads_[i].owner_ == owner){
                    return &threads_[i];
                }
            }
            return nullptr;
        }

        void returnThread(Thread* const t){
            // setting owner_ to nullptr should be unnecessary
            t->inUse_ = false;
        }

    private:

        std::array<Thread, N> threads_;

    };

    static thread_local ThreadPool<BOOST_FAIRNESS_MAX_THREADS> t_;

}
#endif // BOOST_FAIRNESS_THREAD_POOL_HPP
