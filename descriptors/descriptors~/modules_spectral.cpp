
#include "modules_spectral.hpp"
#include "descriptors_graph.hpp"

#include <SIMDSupport.hpp>
#include <Statistics.hpp>

#include <algorithm>
#include <cmath>

using VecType = SIMDType<double, SIMDLimits<double>::max_size>;

// Energy Ratio Module

void module_energy_ratio::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_energy_ratio::calculate(const global_params& params, const double *frame, long size)
{
    // FIX - USES ENERGY OR AMPS?

    const double *power = m_power_module->get_frame();
    long num_bins = m_power_module->num_bins();
            
    m_value = statSumSquares(power + m_min_bin, m_max_bin - m_min_bin) / statSumSquares(power, num_bins);
}

// Spectral Flatness Module

void module_sfm::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_sfm::calculate(const global_params& params, const double *frame, long size)
{
    // FIX - USES ENERGY OR AMPS?

    const double *amplitudes = m_amplitude_module->get_frame();
            
    m_value = statFlatness(amplitudes + m_min_bin, m_max_bin - m_min_bin);
}

// Loudness Module

user_module *module_loudness::setup(const global_params& params, long argc, t_atom *argv)
{
    module_loudness *m = new module_loudness();
    
    m->m_report_db = argc ? atom_getfloat(argv) : true;
    
    return m;
}
    
bool module_loudness::is_the_same(const module *m) const
{
    const module_loudness *m_typed = dynamic_cast<const module_loudness *>(m);
    
    return m_typed && m_typed->m_report_db == m_report_db;
}

void module_loudness::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_loudness::prepare(const global_params& params)
{
    // FIX - check sr
    
    long size = (params.fft_size() >> 1) + 1;
    
    if (m_loudness_curve.size() != size)
    {
        m_loudness_curve.resize((params.fft_size() >> 1) + 1);
        
        double *curve = m_loudness_curve.data();
        
        const double freq_mul = (params.m_sr / 1000.0) / params.fft_size();
        
        for (long i = 0; i < size; i++)
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

    long nyquist = m_power_module->num_bins() - 1;
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
    
    // FIX - requires energy compensation
            
    m_value = m_report_db ? pow_to_db(p) : sqrt(p);
}

// Energy Module

void module_energy::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_energy::calculate(const global_params& params, const double *frame, long size)
{
    const double *power = m_power_module->get_frame();

    const double energy = statSumSquares(power + m_min_bin, m_max_bin - m_min_bin);

    // FIX - requires energy compensation
            
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

    const double crest = statCrest(amps + m_min_bin, m_max_bin - m_min_bin);

    // FIX - check this
            
    m_value = m_report_db ? atodb(crest) : crest;
}

// Rolloff Module

user_module *module_rolloff::setup(const global_params& params, long argc, t_atom *argv)
{
    module_rolloff *m = new module_rolloff();
    
    m->m_centile = (argc ? atom_getfloat(argv) : 0.95) * 100.0;
    
    return m;
}

bool module_rolloff::is_the_same(const module *m) const
{
    const module_rolloff *m_typed = dynamic_cast<const module_rolloff *>(m);
    
    return m_typed && m_typed->m_centile == m_centile;
}

void module_rolloff::add_requirements(graph& g)
{
    m_power_module = g.add_requirement(new module_power_spectrum());
}

void module_rolloff::calculate(const global_params& params, const double *frame, long size)
{
    const double bin = statPDFPercentile(m_power_module->get_frame(), m_centile, m_power_module->num_bins());
    
    m_value = bin * params.bin_freq();
}
    
// Spectral Linear Shape Modules

// Centroid

void module_lin_centroid::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_lin_centroid::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statCentroid(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * params.bin_freq();
}

// Spread

void module_lin_spread::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_lin_spread::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statSpread(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * params.bin_freq();
}

// Skewness

void module_lin_skewness::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_lin_skewness::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statSkewness(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
}

// Kurtosis

void module_lin_kurtosis::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_lin_kurtosis::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statKurtosis(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
}

// Spectral Log Shape Modules

// Centroid

void module_log_centroid::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_log_centroid::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statLogCentroid(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin) * params.bin_freq();
}

// Spread

void module_log_spread::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_log_spread::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statLogSpread(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
}

// Skewness

void module_log_skewness::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_log_skewness::calculate(const global_params& params, const double *frame, long size)
{
    m_value = statLogSkewness(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
}

// Kurtosis

void module_log_kurtosis::add_requirements(graph& g)
{
    m_amplitude_module = g.add_requirement(new module_amplitude_spectrum());
}

void module_log_kurtosis::calculate(const global_params& params, const double *frame, long size) 
{
    m_value = statLogKurtosis(m_amplitude_module->get_frame() + m_min_bin, m_max_bin - m_min_bin);
}
