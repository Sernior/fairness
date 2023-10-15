/**
 * @file control_block_t.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the definition of the control_block_t. TODO
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP
#define BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP
#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <atomic>

namespace boost::fairness{

    /**
     * @brief TODO
     * @private
     */
    struct control_block_64b_t{ 
        /**
         * @brief first bit owned. TODO
         * @private
         */
        int8_t owned_ = 7;
        /**
         * @brief remaining 7 bit is the current priority. TODO
         * @private
         */
        uint8_t priority_[7];
        /**
         * @brief Set the Owned object. TODO
         * @private
         * @return control_block_64b_t 
         */
        control_block_64b_t setOwned() const {
            control_block_64b_t new_ctrl = *this;
            new_ctrl.owned_ |= 0b10000000;
            return new_ctrl;
        }
        /**
         * @brief Increase the priority. TODO
         * @private
         * @param priority 
         * @return control_block_64b_t 
         */
        control_block_64b_t increasePriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            ++new_ctrl.priority_[priority];
            return new_ctrl;
        }
        /**
         * @brief Decrease the priority. TODO
         * @private
         * @param priority 
         * @return control_block_64b_t 
         */
        control_block_64b_t decreasePriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            --new_ctrl.priority_[priority];
            return new_ctrl;
        }
        /**
         * @brief Set the Priority object. TODO
         * @private
         * @param priority 
         * @return control_block_64b_t 
         */
        control_block_64b_t setPriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            new_ctrl.owned_ = priority;
            return new_ctrl;
        }
        /**
         * @brief Get the Priority object. TODO
         * @private
         * @return Priority_t 
         */
        Priority_t getPriority() const {
            return owned_ & 0b01111111;
        }
        /**
         * @brief Get the ownership status. TODO
         * @private
         * @return true : if is owned.
         * @return false : if isn't owned.
         */
        bool isOwned_() const{
            return owned_ < 0;
        }
    };

    #define BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES sizeof(control_block_64b_t::priority_)

    static_assert(std::atomic<control_block_64b_t>::is_always_lock_free, "control_block64b_t is not lock free");

    #ifdef BOOST_FAIRNESS_HAS_DWCAS

    struct control_block_128b_t{ 
        /**
         * @brief 
         * @private
         */
        int8_t owned_ = 15;// first bit owned remaining 7 bit is the current priority
        /**
         * @brief 
         * @private
         */
        uint8_t priority_[15];
        /**
         * @brief 
         * @private
         */
        control_block_128b_t setOwned() const {
            control_block_128b_t new_ctrl = *this;
            new_ctrl.owned_ |= 0b10000000;
            return new_ctrl;
        }
        /**
         * @brief 
         * @private
         */
        control_block_128b_t increasePriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            ++new_ctrl.priority_[priority];
            return new_ctrl;
        }
        /**
         * @brief 
         * @private
         */
        control_block_128b_t decreasePriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            --new_ctrl.priority_[priority];
            return new_ctrl;
        }
        /**
         * @brief 
         * @private
         */
        control_block_128b_t setPriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            new_ctrl.owned_ = priority;
            return new_ctrl;
        }
        /**
         * @brief 
         * @private
         */
        Priority_t getPriority() const {
            return owned_ & 0b01111111;
        }
        /**
         * @brief 
         * @private
         */
        bool isOwned_() const{
            return owned_ < 0;
        }
    };
    #define BOOST_FAIRNESS_SPM128B_SUPPORTED_PRIORITIES sizeof(control_block_128b_t::priority_)

    static_assert(boost::atomic<control_block_128b_t>::is_always_lock_free, "control_block128b_t is not lock free");
    
    #endif // BOOST_FAIRNESS_HAS_DWCAS

} // namespace boost::fairness

#endif // BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP
