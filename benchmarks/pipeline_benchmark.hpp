/**
 * @file pipeline_benchmark.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Alias pipeline_benchmark.
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
#include "std/standard_mutex_benchmark.hpp"
#include "std/standard_shared_mutex_benchmark.hpp"
#include "std/standard_recursive_mutex_benchmark.hpp"
/**
 * @brief This function is used to separate benchmark categories.
 * 
 * @param state : the state of the benchmark.
 */
void ____________________________________________________________________________________________(benchmark::State& state) {
    int i = 0;
    for (auto _ : state){
        ++i;
    }
}
                   