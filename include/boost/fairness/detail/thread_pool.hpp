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

#include <thread>
#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{

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
        std::thread::id owner_{};
        Priority_t priority_{BOOST_FAIRNESS_INVALID_PRIORITY};
        bool inUse_{};
    }; 

    template<size_t N>
    class ThreadPool{
    public:

        ThreadPool() = default;

        Thread* getThread(){
            for (uint32_t i = 0; i < N; ++i){
                if (!threads_[i].inUse_){
                    threads_[i].owner_ = std::this_thread::get_id();
                    return &threads_[i];
                }
            }
            return nullptr;
        }

        Thread* reGetThread(){
            for (uint32_t i = 0; i < N; ++i){
                if (threads_[i].owner_ == std::this_thread::get_id()){
                    return &threads_[i];
                }
            }
            return nullptr;
        }

        void returnThread(Thread* t){
            t->inUse_ = false;
        }

    private:

        //std::array<bool, N> statuses_{};
        std::array<Thread, N> threads_{};
    };

    static thread_local ThreadPool<BOOST_FAIRNESS_MAX_THREADS> t_;

}
#endif // BOOST_FAIRNESS_THREAD_POOL_HPP
