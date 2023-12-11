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
#include <boost/lockfree/queue.hpp>

namespace boost::fairness::detail{
    #define PENDING 1
    #define GRANTED 0

    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{
        std::atomic<uint32_t> state_{PENDING};
        std::atomic<Thread*> watcher_{nullptr};
        std::atomic<Thread*> thread_{nullptr};
        const bool isTail_;

        Request(bool isTail = false) : isTail_{isTail} {};

        void reset(){
            state_ = PENDING;
            watcher_ = nullptr;
            thread_ = nullptr;
        }
    };

    static_assert(sizeof(Request) == BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE, "Request size is not BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE");

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

        bool returnRequest(Request* req){
            req->reset();
            bool ret = requestQueue_.bounded_push(req);
            return ret;
        }

    private:

        std::array<Request, N> reqs_;
        boost::lockfree::queue<Request*, boost::lockfree::capacity<N>> requestQueue_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL_LOCK_HPP
