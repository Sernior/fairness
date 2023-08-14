#pragma once
#include <cstdint>
namespace PrioSync{
    using Priority_t = uint8_t;
    static constexpr Priority_t _max_priority = Priority_t(-1);
}