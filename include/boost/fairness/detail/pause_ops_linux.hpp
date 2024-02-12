//          Copyright © Federico Abrignani 2023 - 2024.
//          Copyright © Salvatore Martorana 2023 - 2024.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt) 

/**
 * @file pause_ops_linux.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief This file contains the implementation of the pause operations used by the mutexes on linux.
 * @version 0.1
 * @date 2023-10-06
 * @copyright Copyright © 2023 - 2024 Federico Abrignani (federignoli@hotmail.it).
 * @copyright Copyright © 2023 - 2024 Salvatore Martorana (salvatoremartorana@hotmail.com).

 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_PAUSE_OPS_LINUX_HPP
#define BOOST_FAIRNESS_PAUSE_OPS_LINUX_HPP

namespace boost::fairness::detail{

#if defined(__x86_64__) || defined(__i386__)
    inline void pause_(){
        __asm__ __volatile__ ("pause");
    }

#elif defined(__ia64__)
    inline void pause_(){
        __asm__ __volatile__ ("hint @pause");
    }

#elif defined(__arm__)
    inline void pause_(){
        __asm__ __volatile__ ("yield");
    }

#else
    inline void pause_(){ // even if pause_ get optimized away it doesn`t matter considering this is used with atomic operations, worst case scenrario the thread does not relax
        return;
    }

#endif

}

#endif // BOOST_FAIRNESS_PAUSE_OPS_LINUX_HPP