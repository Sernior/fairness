#ifndef BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP
#define BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP
#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <atomic>

namespace boost::fairness{

    struct control_block_64b_t{ 
        int8_t owned_ = 7;// first bit owned remaining 7 bit is the current priority
        uint8_t priority_[7];
        control_block_64b_t setOwned() const {
            control_block_64b_t new_ctrl = *this;
            new_ctrl.owned_ |= 0b10000000;
            return new_ctrl;
        }
        control_block_64b_t increasePriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            ++new_ctrl.priority_[priority];
            return new_ctrl;
        }
        control_block_64b_t decreasePriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            --new_ctrl.priority_[priority];
            return new_ctrl;
        }
        control_block_64b_t setPriority(Priority_t const priority) const {
            control_block_64b_t new_ctrl = *this;
            new_ctrl.owned_ = priority;
            return new_ctrl;
        }
        Priority_t getPriority() const {
            return owned_ & 0b01111111;
        }
    };

    #define BOOST_FAIRNESS_SPM64B_SUPPORTED_PRIORITIES sizeof(control_block_64b_t::priority_)

    static_assert(std::atomic<control_block_64b_t>::is_always_lock_free, "control_block64b_t is not lock free");

#ifdef BOOST_FAIRNESS_HAS_DWCAS
    struct control_block_128b_t{ 
        int8_t owned_ = 15;// first bit owned remaining 7 bit is the current priority
        uint8_t priority_[15];
        control_block_128b_t setOwned() const {
            control_block_128b_t new_ctrl = *this;
            new_ctrl.owned_ |= 0b10000000;
            return new_ctrl;
        }
        control_block_128b_t increasePriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            ++new_ctrl.priority_[priority];
            return new_ctrl;
        }
        control_block_128b_t decreasePriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            --new_ctrl.priority_[priority];
            return new_ctrl;
        }
        control_block_128b_t setPriority(Priority_t const priority) const {
            control_block_128b_t new_ctrl = *this;
            new_ctrl.owned_ = priority;
            return new_ctrl;
        }
        Priority_t getPriority() const {
            return owned_ & 0b01111111;
        }
    };
    #define BOOST_FAIRNESS_SPM128B_SUPPORTED_PRIORITIES sizeof(control_block_64b_t::priority_)

    static_assert(boost::atomic<control_block_128b_t>::is_always_lock_free, "control_block128b_t is not lock free");
#endif // BOOST_FAIRNESS_HAS_DWCAS

}

#endif // BOOST_FAIRNESS_CONTROL_BLOCK_T_HPP