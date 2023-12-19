/**
 * @file request_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana
 * @brief This file contains the implementation of a static very fast and simple request pool.
 * @version 0.1
 * @date 2023-12-11
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_REQUEST_POOL_HPP
#define BOOST_FAIRNESS_REQUEST_POOL_HPP

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

        /*
        maybe it would be better to have a separate atomic flag array for "inUse" somewhere else instead to even better respect
        cache coherency... or maybe not... keep an eye here!
        */
        std::atomic_flag inUse{};

        Request() {};

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
        // lockfree stack is too slow for me I need something simpler and faster
        boost::lockfree::stack<Request*, boost::lockfree::capacity<N>> requestQueue_;
    };
*/
    template<size_t N>
    class RequestPool{
    public:
        RequestPool() = default;

        Request* getRequest(){
            for (uint32_t i = 0; i < N; ++i){
                if (!reqs_[i].inUse.test() && !reqs_[i].inUse.test_and_set())
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
#endif // BOOST_FAIRNESS_REQUEST_POOL_HPP
