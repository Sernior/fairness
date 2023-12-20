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
        const size_t index_;
        std::atomic_flag inUse_{};

        Request(size_t idx) : index_(idx) {};

        void reset(){
            state_.store(PENDING, std::memory_order_relaxed);
            watcher_ = nullptr;
            thread_ = nullptr;
        }
    };

    static_assert(sizeof(Request) == BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE, "Request size is not BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE");

    template <size_t... Is>
    auto make_requests(std::index_sequence<Is...>) {
        return std::array<Request, sizeof...(Is)>{Request(Is)...};
    }

    template<size_t N>
    class RequestPool{
    public:

        RequestPool() : reqs_(make_requests(std::make_index_sequence<N>())) {}

        Request* getRequest(){
            for (uint32_t i = 0; i < N; ++i){
                if (!reqs_[i].inUse_.test(std::memory_order_relaxed) && !reqs_[i].inUse_.test_and_set(std::memory_order_acquire))
                    return &reqs_[i];
            }
            return nullptr;
        }

        void returnRequest(Request* req){
            req->reset();
            req->inUse_.clear(std::memory_order_release);
        }

    private:
        std::array<Request, N> reqs_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL_HPP
