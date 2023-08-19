/**
 * @file priority_t.h
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author P. Di Giglio
 * @author S. Martorana
 * @brief #TODO
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include <cstdint>
namespace PrioSync{
    using Priority_t = uint8_t;
    static constexpr Priority_t _max_priority = Priority_t(-1);
}
