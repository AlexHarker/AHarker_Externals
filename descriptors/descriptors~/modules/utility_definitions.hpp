
#ifndef _UTILITY_DEFINITIONS_HPP_
#define _UTILITY_DEFINITIONS_HPP_

#include <limits>
#include <SIMDSupport.hpp>

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

#endif /* _UTILITY_DEFINITIONS_HPP_ */
