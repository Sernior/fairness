/**
 * @file #TODO.hpp
 * @author S. Martorana
 * @brief Alias #TODO.
 * @version 0.1
 * @date 2023-10-06
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */

#include "experimental/experimental_mutex_benchmark.hpp"
#include "pm/priority_mutex_benchmark.hpp"
#include "rpm/recursive_priority_mutex_benchmark.hpp"
#include "slm/slim_priority_mutex_benchmark.hpp"
#include "spm/shared_priority_mutex_benchmark.hpp"
#include "spnlcpm/spinlock_priority_mutex_benchmark.hpp"
#include "std/standard_benchmark.hpp"
#include "std/standard_shared_benchmark.hpp"

void ____________________________________________________________________________________________(benchmark::State& state) {
    int i = 0;
    for (auto _ : state){
        ++i;
    }
}
                   