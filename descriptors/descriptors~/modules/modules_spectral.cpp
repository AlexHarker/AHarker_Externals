
#include "modules_spectral.hpp"
#include "descriptors_graph.hpp"

#include <SIMDSupport.hpp>
#include <Statistics.hpp>

#include <algorithm>
#include <cmath>

#include <limits>

static constexpr double infinity() { return std::numeric_limits<double>::infinity(); }

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// Energy Ratio Module

void module_energy_ratio::add_requirements(graph& g)
{
    m_power_sum_module = g.add_requirement(new module_power_sum());
}

void module_energy_ratio::calculate(const global_params& params, const double *frame, long size)
{
    const double band = m_power_sum_module->get_sum(m_min_bin, m_max_bin);
    const double energy = m_power_sum_module->get_sum(0, params.num_bins());
            
    m_value = energy ? band / energy : infinity();
}

// Spectral Flatness Module

void module_sfm::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    m_amplitude_sum_module = g.add_requirement(new module_amplitude_sum());
}

void module_sfm::calculate(const global_params& params, const double *frame, long size)
{
    // FIX - Currently uses amplitudes (does not match previous version)

    const double *amplitudes = m_amplitude_module->get_frame();
    const double mean = m_amplitude_sum_module->get_sum(m_min_bin, m_max_bin) / bin_count();
    
    m_value = mean ? stat_geometric_mean(amplitudes + m_min_bin, bin_count()) / mean : infinity();
}

// Loudness Module

user_module *module_loudness::setup(const global_params& params, module_arguments& args)
{
    return  new module_loudness(args.get_bool(true));
}

void module_loudness::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_loudness::prepare(const global_params& params)
{    
    long num_bins = params.num_bins();
    
    if (m_loudness_curve.size() != num_bins || m_sr != params.m_sr)
    {
        m_loudness_curve.resize(num_bins);
        m_sr = params.m_sr;
        
        double *curve = m_loudness_curve.data();
        
        const double freq_mul = (params.m_sr / 1000.0) / params.fft_size();
        
        for (long i = 0; i < num_bins; i++)
        {
            const double f = i * freq_mul;
            const double fo = f - 3.3;
            const double fo2 = fo * fo;
            const double f2 = f * f;
            const double f4 = f2 * f2;
            const double a = pow(10.0, (-3.64 * pow(f, -0.8) + 6.5 * exp(-0.6 * fo2) - (0.001 * f4)) / 20.0);
            
            curve[i] = a * a;
        }
    }
}

void module_loudness::calculate(const global_params& params, const double *frame, long size)
{
    const double *curve_frame = m_loudness_curve.data();
    const double *power_frame = m_power_module->get_frame();
            
    const VecType *curve = reinterpret_cast<const VecType *>(curve_frame);
    const VecType *power = reinterpret_cast<const VecType *>(power_frame);
    VecType sum(0.0);

    long nyquist = params.num_bins() - 1;
    long loop_size = nyquist / VecType::size;
    
    // Calculate amplitude spectrum
    
    for (long i = 0; i < loop_size; i++)
        sum += power[i] * curve[i];
    
    // Nyquist Value
   
    double p = power_frame[nyquist] * curve_frame[nyquist];

    // Sum
    
    double store_sum[VecType::size];
    sum.store(store_sum);
    
    for (int i = 0; i < VecType::size; i++)
        p += store_sum[i];
    
    p *= m_power_module->get_energy_compensation();
                
    m_value = m_report_db ? pow_to_db(p) : sqrt(p);
}

// Energy Module

void module_energy::add_requirements(graph& g)
{
    m_power_sum_module = g.add_requirement(new module_power_sum());
}

void module_energy::calculate(const global_params& params, const double *frame, long size)
{
    double energy = m_power_sum_module->get_sum(m_min_bin, m_max_bin);

    energy *= m_power_sum_module->get_energy_compensation();
            
    m_value = m_report_db ? pow_to_db(energy) : energy;
}

// Spectral Crest Module

void module_spectral_crest::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_spectral_crest::calculate(const global_params& params, const double *frame, long size)
{
    const double *amps = m_amplitude_module->get_frame();

    // N.B. - This doesn't use stat_crest as the denominator is simply the mean

    const double max = stat_max(amps + m_min_bin, m_max_bin - m_min_bin);
    const double mean = stat_mean(amps + m_min_bin, m_max_bin - m_min_bin);
    const double crest = mean ? max / mean : infinity();
            
    m_value = m_report_db ? atodb(crest) : crest;
}

// Rolloff Module

user_module *module_rolloff::setup(const global_params& params, module_arguments& args)
{
    return new module_rolloff(args.get_double(0.95, 0.0, 1.0));
}

void module_rolloff::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
    m_power_sum_module = g.add_requirement(new module_power_sum());
}

void module_rolloff::calculate(const global_params& params, const double *frame, long size)
{
    const double *power_frame = m_power_module->get_frame();
    
    const double target = m_power_sum_module->get_sum(0, params.num_bins()) * m_ratio;
    
    double sum = 0.0;
    double bin = params.num_bins();
    
    for (long i = 0; i < params.num_bins(); i++)
    {
        sum += power_frame[i];
        if (sum >= target)
        {
            bin = static_cast<double>(i - ((sum - target) / power_frame[i]));
            break;
        }
    }
        
    m_value = target ? bin * params.bin_freq() :  infinity();
}
    
// Spectral Linear Shape Modules

// Centroid

void module_lin_centroid::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    m_amplitude_sum_module = g.add_requirement(new module_amplitude_sum());
}

void module_lin_centroid::calculate(const global_params& params, const double *frame, long size)
{
    const double *data = m_amplitude_module->get_frame() + m_min_bin;
    
    m_sum = m_amplitude_sum_module->get_sum(m_min_bin, m_max_bin);
    m_raw = m_sum ? stat_weighted_sum(data, bin_count()) / m_sum : infinity();
    m_value = (m_raw + m_min_bin) * params.bin_freq();
}

// Spread

void module_lin_spread::add_requirements(graph& g)
{
    m_centroid_module = g.add_requirement(new module_lin_centroid(m_lo_freq, m_hi_freq));
}

void module_lin_spread::calculate(const global_params& params, const double *frame, long size)
{
    using calculation = impl::indices_diff_op<impl::pow2>;
    
    const double *data = m_centroid_module->get_frame() + m_min_bin;
    
    const double centroid = m_centroid_module->get_raw_centroid();
    const double sum = m_centroid_module->get_sum();

    m_raw = sum ? sqrt(stat_weighted_sum(calculation(centroid), data, bin_count()) / sum) : infinity();
    m_value = m_raw * params.bin_freq();
}

// Skewness

void module_lin_skewness::add_requirements(graph& g)
{
    m_spread_module = g.add_requirement(new module_lin_spread(m_lo_freq, m_hi_freq));
}

void module_lin_skewness::calculate(const global_params& params, const double *frame, long size)
{
    using calculation = impl::indices_diff_op<impl::pow3>;

    const double *data = m_spread_module->get_frame() + m_min_bin;
    
    const double centroid = m_spread_module->get_raw_centroid();
    const double spread = m_spread_module->get_raw_spread();
    const double sum = m_spread_module->get_sum();
    
    double denominator = impl::pow3()(spread) * sum;
    m_value = sum ? stat_weighted_sum(calculation(centroid), data, bin_count()) / denominator : infinity();
}

// Kurtosis

void module_lin_kurtosis::add_requirements(graph& g)
{
    m_spread_module = g.add_requirement(new module_lin_spread(m_lo_freq, m_hi_freq));
}

void module_lin_kurtosis::calculate(const global_params& params, const double *frame, long size)
{
    using calculation = impl::indices_diff_op<impl::pow4>;

    const double *data = m_spread_module->get_frame() + m_min_bin;

    const double centroid = m_spread_module->get_raw_centroid();
    const double spread = m_spread_module->get_raw_spread();
    const double sum = m_spread_module->get_sum();
    
    double denominator = impl::pow4()(spread) * sum;
    m_value = sum ? stat_weighted_sum(calculation(centroid), data, bin_count()) / denominator : infinity();
}

// Spectral Log Shape Modules

// Centroid

void module_log_centroid::add_requirements(graph& g)
{
    m_log_bins_module = g.add_requirement(new module_log_bins());
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
    m_amplitude_sum_module = g.add_requirement(new module_amplitude_sum());
}

void module_log_centroid::calculate(const global_params& params, const double *frame, long size)
{
    const double *data = m_amplitude_module->get_frame() + m_min_bin;
    const double *log_bins = m_log_bins_module->get_log_bins() + m_min_bin;

    m_sum = m_amplitude_sum_module->get_sum(m_min_bin, m_max_bin);
    m_raw = m_sum ? stat_weighted_sum(log_bins, data, bin_count()) / m_sum : infinity();
    m_value = exp2(m_raw + m_min_bin) * params.bin_freq();
}

// Spread

void module_log_spread::add_requirements(graph& g)
{
    m_centroid_module = g.add_requirement(new module_log_centroid(m_lo_freq, m_hi_freq));
}

void module_log_spread::calculate(const global_params& params, const double *frame, long size)
{
    using calculation = impl::modified_diff_data<const double *, impl::pow2>;
    
    const double *data = m_centroid_module->get_frame() + m_min_bin;
    const double *log_bins = m_centroid_module->get_log_bins() + m_min_bin;

    const double centroid = m_centroid_module->get_raw_centroid();
    const double sum = m_centroid_module->get_sum();
    
    m_raw = sum ? sqrt(stat_weighted_sum(calculation(log_bins, centroid), data, bin_count()) / sum) : infinity();
    m_value = m_raw;
}

// Skewness

void module_log_skewness::add_requirements(graph& g)
{
    m_spread_module = g.add_requirement(new module_log_spread(m_lo_freq, m_hi_freq));
}

void module_log_skewness::calculate(const global_params& params, const double *frame, long size)
{
    using calculation = impl::modified_diff_data<const double *, impl::pow3>;
    
    const double *data = m_spread_module->get_frame() + m_min_bin;
    const double *log_bins = m_spread_module->get_log_bins() + m_min_bin;

    const double centroid = m_spread_module->get_raw_centroid();
    const double spread = m_spread_module->get_raw_spread();
    const double sum = m_spread_module->get_sum();
    
    double denominator = impl::pow3()(spread) * sum;
    m_value = sum ? stat_weighted_sum(calculation(log_bins, centroid), data, bin_count()) / denominator : infinity();
}

// Kurtosis

void module_log_kurtosis::add_requirements(graph& g)
{
    m_spread_module = g.add_requirement(new module_log_spread(m_lo_freq, m_hi_freq));
}

void module_log_kurtosis::calculate(const global_params& params, const double *frame, long size) 
{
    using calculation = impl::modified_diff_data<const double *, impl::pow4>;
    
    const double *data = m_spread_module->get_frame() + m_min_bin;
    const double *log_bins = m_spread_module->get_log_bins() + m_min_bin;

    const double centroid = m_spread_module->get_raw_centroid();
    const double spread = m_spread_module->get_raw_spread();
    const double sum = m_spread_module->get_sum();
    
    double denominator = impl::pow4()(spread) * sum;
    m_value = sum ? stat_weighted_sum(calculation(log_bins, centroid), data, bin_count()) / denominator : infinity();
}
