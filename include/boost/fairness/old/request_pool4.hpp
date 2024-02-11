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
#ifndef BOOST_FAIRNESS_REQUEST_POOL2_HPP
#define BOOST_FAIRNESS_REQUEST_POOL2_HPP

#include <atomic>
#include <array>
#include <boost/fairness/config.hpp>

namespace boost::fairness::detail::old{
/*
    struct Thread;

    struct alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) Request{
        Thread* watcher_{nullptr};
        Thread* thread_{nullptr};
        std::atomic<uint32_t> state_{PENDING};
        std::atomic_flag inUse_{};

        Request() = default;

        void reset(){
            state_.store(PENDING, std::memory_order_relaxed);
            watcher_ = nullptr;
            thread_ = nullptr;
        }
    };

    static_assert(sizeof(Request) == BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE, "Request size is not BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE");
*/

    //template <size_t... Is>
    //auto make_requests(std::index_sequence<Is...>) {
    //    return std::array<Request, sizeof...(Is)>{Request(Is)...};
    //}

    template<size_t N>
    class RequestPool4{
    public:

        RequestPool4() = default;

        //RequestPool() : reqs_(make_requests(std::make_index_sequence<N>())) {}

        Request* getRequest(){
            for (uint32_t i = 0; i < N; ++i){
                if (!reqs_[i].inUse_.test(std::memory_order_relaxed) && !reqs_[i].inUse_.test_and_set(std::memory_order_acquire))
                    return &reqs_[i];
                spin_wait();
            }
            return nullptr;
        }

        void returnRequest(Request* const req){
            req->reset();
            req->inUse_.clear(std::memory_order_release);
            //statuses_[req->index_].clear(std::memory_order_release);
        }

    private:
        std::array<Request, N> reqs_{};
        //alignas(BOOST_FAIRNESS_HARDWARE_DESTRUCTIVE_SIZE) std::array<std::atomic_flag, N> statuses_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL2_HPP
