/**
 * @file acquisition_modes.hpp
 * @author F. Abrignani (federignoli@hotmail.it)
 * @author S. Martorana (salvatoremartorana@hotmail.com)
 * @brief Empty types used by unique_lock and shared_lock constructors.
 * @version 0.1
 * @date 2023-08-19
 * @private
 * @copyright Copyright (c) 2023 Federico Abrignani (federignoli@hotmail.it).
 * Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
 * 
 */
#ifndef BOOST_FAIRNESS_ACQUISITION_MODES_HPP
#define BOOST_FAIRNESS_ACQUISITION_MODES_HPP

namespace boost::fairness{

  /**
   * @brief structs used to overload unique_lock and shared_lock methods
   * @private
   */
  struct defer_lock_t { explicit defer_lock_t() = default; };
  /**
   * @brief 
   * @private
   */
  struct try_to_lock_t { explicit try_to_lock_t() = default; };
  /**
   * @brief 
   * @private
   */
  struct adopt_lock_t { explicit adopt_lock_t() = default; };
  /**
   * @brief 
   * @private
   */
  static constexpr defer_lock_t defer_lock;
  /**
   * @brief 
   * @private
   */
  static constexpr try_to_lock_t try_to_lock;
  /**
   * @brief 
   * @private
   */
  static constexpr adopt_lock_t adopt_lock;

}
#endif // BOOST_FAIRNESS_ACQUISITION_MODES_HPP
