/**
 * @file request_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a request pool based on boost::lockfree queue.
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_REQUEST_POOL_LOCK_HPP
#define BOOST_FAIRNESS_REQUEST_POOL_LOCK_HPP

#include <atomic>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail{
    #define PENDING 1
    #define GRANTED 0

    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{
        std::atomic<uint32_t> state_{PENDING};
        Thread* watcher_{nullptr};
        Thread* thread_{nullptr};
        const bool isFirstTail_;

        /*
        maybe it would be better to have a separate atomic flag array somewhere else instead to even better respect
        cache coherency. TODO
        */
        std::atomic_flag inUse{};

        Request(bool isFirstTail = false) : isFirstTail_{isFirstTail} {};

        void reset(){
            state_.store(PENDING);
            watcher_ = nullptr;
            thread_ = nullptr;
        }
    };

    static_assert(sizeof(Request) == BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE, "Request size is not BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE");

/*
    template<size_t N>
    class RequestPool{
    public:
        RequestPool(){
            for (auto& req : reqs_)
                requestQueue_.bounded_push(&req);
        }

        Request* getRequest(){
            Request* ret;
            if (requestQueue_.pop(ret))
                return ret;
            return nullptr;
        }

        void returnRequest(Request* req){
            req->reset();
            requestQueue_.bounded_push(req);
        }

    private:

        std::array<Request, N> reqs_;
        boost::lockfree::stack<Request*, boost::lockfree::capacity<N>> requestQueue_;
    };
*/
    template<size_t N>
    class RequestPool{
    public:
        RequestPool() = default;

        Request* getRequest(){
            for (uint32_t i = 0; i < N; ++i){
                if (!reqs_[i].inUse.test_and_set()) // instead of directly calling test_and_set I should TATAS this one TODO!
                    return &reqs_[i];
            }
            return nullptr;
        }

        void returnRequest(Request* req){
            req->reset();
            req->inUse.clear();
        }

    private:

        std::array<Request, N> reqs_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL_LOCK_HPP
