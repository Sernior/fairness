/**
 * @file slim_priority_mutex.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief This file contains the implementation of the slim_priority_mutex.
 * @version 0.1
 * @date 2023-08-19
 * 
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#ifndef BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
#define BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
#include <thread>
#include <atomic>
#include <array>
#include <chrono>
#include <thread>
#include <mutex>
#include <boost/fairness/priority_t.hpp>
#include <cstring>
#include <boost/fairness/config.hpp>

namespace boost::fairness{

    template<bool> struct Range;

    template<size_t N, typename = Range<true> >
    struct slim_priority_mutex{};

    /**
     * @brief The slim_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the slim_priority_mutex manages, up to 7.
     */
    template<size_t N>
    class slim_priority_mutex<N, Range<(1 <= N && N <= 7)>>{

        //NO DWCAS
        struct control_block_t{ 
            int8_t owned_ = 7;// first bit owned remaining 7 bit is the current priority
            uint8_t priority_[7];
            control_block_t setOwned() const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ |= 0b10000000;
                return new_ctrl;
            }
            control_block_t increasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                ++new_ctrl.priority_[priority];
                return new_ctrl;
            }
            control_block_t decreasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                --new_ctrl.priority_[priority];
                return new_ctrl;
            }
            control_block_t setPriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ = priority;
                return new_ctrl;
            }
            Priority_t getPriority() const {
                return owned_ & 0b01111111;
            }
        };

        static_assert(std::atomic<control_block_t>::is_always_lock_free, "control_block_t is not lock free");

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        slim_priority_mutex() = default;

        /// @private
        slim_priority_mutex(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex& operator=(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex(slim_priority_mutex&&) = delete;

        /// @private
        slim_priority_mutex& operator=(slim_priority_mutex&&) = delete;

        /// @private
        ~slim_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex, blocking the thread if the slim_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * slim_priority_mutex<7> m;
         * BOOST_FAIRNESS_CONFIG_HPPo);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){

            control_block_t localCtrl = ctrl_.load();

            // try to increment the waiters counter in this priority
            for(;;){
                // if we risk an overflow lets spin instead
                if (localCtrl.priority_[priority] == uint8_t(-1)){
                    localCtrl = ctrl_.load();
                    continue;
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.increasePriority(priority))){
                    break;
                }
            }
            // if we can proceed we do and decrement the counter otherwise we wait on an atomic_flag
            for(;;){
                localCtrl = ctrl_.load();
                if (!lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned())){
                    break;
                }
                waiting_flags_[priority].wait(false);
            }

            localCtrl = ctrl_.load();

            for(;;){
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.decreasePriority(priority))){
                    break;
                }
            }

        }

        /**
         * @brief Release the slim_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * slim_priority_mutex<7> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            Priority_t localFirstPriority;
            control_block_t localCtrl;
            for (;;){
                reset_();
                localCtrl = ctrl_.load();
                localFirstPriority = find_first_priority_(localCtrl);
                if (localFirstPriority < N){
                    waiting_flags_[localFirstPriority].test_and_set();
                    waiting_flags_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.setPriority(localFirstPriority)))
                    break;
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * slim_priority_mutex<7> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){
            control_block_t localCtrl = ctrl_.load();
            return !lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned());
        }

        private:
        std::array<std::atomic_flag, N> waiting_flags_;
        std::atomic<control_block_t> ctrl_;

        void reset_(){ // there probably is a much better way to do this
            std::memset(&waiting_flags_, 0b00000000, N); // maybe undefined because lock is reading on the waits? Should be ok.
        }

        Priority_t find_first_priority_(control_block_t const& ctrl){
            for (Priority_t i = 0; i < N; ++i){
                if (ctrl.priority_[i] > 0)
                    return i;
            }
            return 7;
        }

        bool lockOwned_(int8_t ctrl) const{
            return ctrl < 0;
        }

    };

#ifdef BOOST_FAIRNESS_HAS_DWCAS

    /**
     * @brief The slim_priority_mutex is an advanced synchronization mechanism that enhances the traditional mutex by introducing a priority-based approach.
     * 
     * @tparam N : number of 0 indexed priorities the slim_priority_mutex manages, up to 15.
     */
    template<size_t N>
    class slim_priority_mutex<N, Range<(8 <= N && N <= 15)>>{

        struct control_block_t{ 
            int8_t owned_ = 15;// first bit owned remaining 7 bit is the current priority
            uint8_t priority_[15];
            control_block_t setOwned() const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ |= 0b10000000;
                return new_ctrl;
            }
            control_block_t increasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                ++new_ctrl.priority_[priority];
                return new_ctrl;
            }
            control_block_t decreasePriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                --new_ctrl.priority_[priority];
                return new_ctrl;
            }
            control_block_t setPriority(Priority_t const priority) const {
                control_block_t new_ctrl = *this;
                new_ctrl.owned_ = priority;
                return new_ctrl;
            }
            Priority_t getPriority() const {
                return owned_ & 0b01111111;
            }
        };

        static_assert(boost::atomic<control_block_t>::is_always_lock_free, "control_block_t is not lock free");

        using Thread_cnt_t = uint32_t;

        public:

        /// @private
        slim_priority_mutex() = default;

        /// @private
        slim_priority_mutex(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex& operator=(const slim_priority_mutex&) = delete;

        /// @private
        slim_priority_mutex(slim_priority_mutex&&) = delete;

        /// @private
        slim_priority_mutex& operator=(slim_priority_mutex&&) = delete;

        /// @private
        ~slim_priority_mutex() = default;

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex, blocking the thread if the slim_priority_mutex was already owned or other threads are waiting with higher priority.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * slim_priority_mutex<15> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.lock(prio);
         *      //...some code.
         * }
         * \endcode
         */
        void lock(Priority_t const priority = 0){

            control_block_t localCtrl = ctrl_.load();

            // try to increment the waiters counter in this priority
            for(;;){
                // if we risk an overflow lets spin instead
                if (localCtrl.priority_[priority] == uint8_t(-1)){
                    localCtrl = ctrl_.load();
                    continue;
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.increasePriority(priority))){
                    break;
                }
            }
            // if we can proceed we do and decrement the counter otherwise we wait on an atomic_flag
            for(;;){
                localCtrl = ctrl_.load();
                if (!lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned())){
                    break;
                }
                waiting_flags_[priority].wait(false);
            }

            localCtrl = ctrl_.load();

            for(;;){
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.decreasePriority(priority))){
                    break;
                }
            }

        }

        /**
         * @brief Release the slim_priority_mutex from unique ownership.
         * 
         * \code{.cpp}
         * slim_priority_mutex<7> m;
         * 
         * void my_function() {
         *      //...some code.
         *      m.unlock();
         *      //...some code.
         * }
         * \endcode
         */
        void unlock(){
            Priority_t localFirstPriority;
            control_block_t localCtrl;
            for (;;){
                reset_();
                localCtrl = ctrl_.load();
                localFirstPriority = find_first_priority_(localCtrl);
                if (localFirstPriority < N){
                    waiting_flags_[localFirstPriority].test_and_set();
                    waiting_flags_[localFirstPriority].notify_one();
                }
                if (ctrl_.compare_exchange_weak(localCtrl, localCtrl.setPriority(localFirstPriority)))
                    break;
            }
        }

        /**
         * @brief Try to acquire the unique ownership of the slim_priority_mutex, if successful will return true, false otherwise.
         * 
         * @param priority used to set a priority for this thread to aquire the lock.
         * 
         * \code{.cpp}
         * slim_priority_mutex<15> m;
         * 
         * void my_function(int prio) {
         *      //...some code.
         *      m.try_lock(prio);
         *      //...some code.
         * }
         * \endcode
         * @return bool 
         */
        [[nodiscard]] bool try_lock(Priority_t const priority = 0){
            control_block_t localCtrl = ctrl_.load();
            return !lockOwned_(localCtrl.owned_) && priority <= localCtrl.owned_ && ctrl_.compare_exchange_strong(localCtrl, localCtrl.setOwned());
        }

        private:
        std::array<boost::atomic_flag, N> waiting_flags_;
        boost::atomic<control_block_t> ctrl_;

        void reset_(){ // there probably is a much better way to do this
            std::memset(&waiting_flags_, 0b00000000, N); // maybe undefined because lock is reading on the waits? Should be ok.
        }

        Priority_t find_first_priority_(control_block_t const& ctrl){
            for (Priority_t i = 0; i < N; ++i){
                if (ctrl.priority_[i] > 0)
                    return i;
            }
            return 15;
        }

        bool lockOwned_(int8_t ctrl) const{
            return ctrl < 0;
        }

    };

#endif // BOOST_FAIRNESS_HAS_DWCAS

}
#endif // BOOST_FAIRNESS_SLIM_PRIORITY_MUTEX_HPP
