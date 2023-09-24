
#ifndef BOOST_FAIRNESS_HPP
#define BOOST_FAIRNESS_HPP

#include <boost/fairness/config.hpp>
#include <boost/fairness/priority_t.hpp>
#include <boost/fairness/priority_mutex.hpp>
#include <boost/fairness/shared_priority_mutex.hpp>
#include <boost/fairness/slim_priority_mutex.hpp>
#include <boost/fairness/spinlock_priority_mutex.hpp>

#ifdef BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES
#include <boost/fairness/experimental/experimental_priority_mutex.hpp>
#include <boost/fairness/experimental/slim_spinlock_priority_mutex.hpp>
#endif // BOOST_FAIRNESS_EXPERIMENTAL_MUTEXES

#endif // BOOST_FAIRNESS_HPP
