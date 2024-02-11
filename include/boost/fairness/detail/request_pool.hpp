/**
 * @file request_pool.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
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
#include <array>
#include <boost/fairness/config.hpp>
#include <boost/fairness/detail/bounded_mpmc_queue.hpp>

namespace boost::fairness::detail{
    #define PENDING 1
    #define GRANTED 0

    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{
        Thread* watcher_{nullptr};
        Thread* thread_{nullptr};
        std::atomic<uint32_t> state_{PENDING};

        Request() = default;

        void reset(){
            state_.store(PENDING, std::memory_order_relaxed);
            watcher_ = nullptr;
            thread_ = nullptr;
        }
    };

    static_assert(sizeof(Request) == BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE, "Request size is not BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE");

    template<size_t N>
    class RequestPool{
    public:

        RequestPool(){
            for (auto& req : requests_)
                freePool_.enqueue(&req);
        }


        Request* getRequest(){
            Request* req;
            if (freePool_.dequeue(req))
                return req;
            return nullptr;
        }

        void returnRequest(Request* const req){
            req->reset();
            freePool_.enqueue(req);
        }

    private:
        std::array<Request, N> requests_;
        boost::fairness::detail::mpmc_bounded_queue<Request*, N> freePool_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL_HPP
