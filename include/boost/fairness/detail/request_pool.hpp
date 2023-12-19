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

        Request(size_t idx) : index_(idx) {};

        void reset(){
            state_.store(PENDING);
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
                if (!statuses_[i].test() && !statuses_[i].test_and_set())
                    return &reqs_[i];
            }
            return nullptr;
        }

        void returnRequest(Request* req){
            req->reset();
            statuses_[req->index_].clear();
        }

    private:
    /*
    I know what you are thinking... you are thinking why did I put index_ to use for statuses_ as a member of Request instead of an atomic_flag directly?
    To avoid false sharing an atomic flag with state_.
    */
        std::array<std::atomic_flag, N> statuses_;
        std::array<Request, N> reqs_;
    };

}
#endif // BOOST_FAIRNESS_REQUEST_POOL_HPP
