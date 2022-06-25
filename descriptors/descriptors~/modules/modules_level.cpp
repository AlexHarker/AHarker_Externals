
#include "modules_level.hpp"

#include <Statistics.hpp>

// Abs Module

void module_average_abs_amp::calculate(const global_params& params, const double *frame, long size)
{
    set(stat_sum_abs(frame, size) / size);
}

// RMS Module

void module_average_rms_amp::calculate(const global_params& params, const double *frame, long size)
{
    set(stat_rms(frame, size));
}

// Peak Amp Module

void module_peak_amp::calculate(const global_params& params, const double *frame, long size)
{
    set(stat_max(frame, size));
}
