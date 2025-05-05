
#ifndef _UTILITY_DEFINITIONS_HPP_
#define _UTILITY_DEFINITIONS_HPP_

#include <limits>
#include <simd_support.hpp>

using VecType = htl::simd_type<double, htl::simd_limits<double>::max_size>;

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

#endif /* _UTILITY_DEFINITIONS_HPP_ */
