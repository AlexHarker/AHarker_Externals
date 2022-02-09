
/*
 *  descriptors_constants.h
 *
 *	This header file contains all the constants and enums for the descriptors~ and descriptorsrt~ objects.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_CONSTANTS_
#define _DESCRIPTORS_CONSTANTS_

#include <ext.h>

// Constants

#define RING_BUFFER_SIZE 33
#define MAX_N_SEARCH 4096

// DB limits

#define DB_MIN -180.
#define DB_EPSILON -300.
#define DB_MAX_MKL_EQUAL -140.
extern float POW_MIN;
extern float LOG_MIN;
extern double MKL_EQUALISE_MAX_LOG;

// Descriptor flag defines

#define DO_MEAN 1
#define DO_STDD 2
#define DO_RANGE 4
#define DO_MEDIAN 8
#define DO_RATIO_ABOVE 16
#define DO_RATIO_BELOW 32

// PI defines

#define FFTW_TWOPI		 6.28318530717958647692

// FFT size defines

#define DEFAULT_MAX_FFT_SIZE_LOG2 16
#define MIN_FFT_SIZE_LOG2 4
#define MAX_FFT_SIZE_LOG2 20

// Descriptor variables and enums

#define MAX_PF_PARAMS 768
#define MAX_PF_CALC 512
#define	MAX_PF_OUTPUT_PARAMS 512
#define	MAX_OUTPUT 4096

#define	MAX_PB_PARAMS 32
#define	MAX_PB_POS 16

// Note that teh below values are specified to ensure that they can be uniquely combined by making one set negative
enum PFDescriptorType {

	DESCRIPTOR_PF_NONE					= 0,
	DESCRIPTOR_PF_ENERGY				= 1,
	DESCRIPTOR_PF_ENERGY_RATIO			= 2,
	DESCRIPTOR_PF_SPECTRAL_ROLLOFF		= 3,
	DESCRIPTOR_PF_SPECTRAL_CREST		= 4,
	DESCRIPTOR_PF_FLUX					= 5,
	DESCRIPTOR_PF_MKL					= 6,
	DESCRIPTOR_PF_FOOTE					= 7,
	DESCRIPTOR_PF_AVERAGE_AMP_ABS		= 8,
	DESCRIPTOR_PF_AVERAGE_AMP_RMS		= 9,
	DESCRIPTOR_PF_PEAK_AMP				= 10,
	DESCRIPTOR_PF_LOUDNESS				= 11,
	DESCRIPTOR_PF_CENTROID_LIN			= 12,
	DESCRIPTOR_PF_SPREAD_LIN			= 13,
	DESCRIPTOR_PF_SKEWNESS_LIN			= 14,
	DESCRIPTOR_PF_KURTOSIS_LIN			= 15,
	DESCRIPTOR_PF_CENTROID_LOG			= 16,
	DESCRIPTOR_PF_SKEWNESS_LOG			= 17,
	DESCRIPTOR_PF_KURTOSIS_LOG			= 18,
	DESCRIPTOR_PF_SPREAD_LOG			= 19,
	DESCRIPTOR_PF_BRIGHTNESS_LIN		= 20,
	DESCRIPTOR_PF_BRIGHTNESS_LOG		= 21,
	DESCRIPTOR_PF_SPECTRAL_FLATNESS		= 22,
	DESCRIPTOR_PF_NOISE_RATIO			= 23,
	DESCRIPTOR_PF_NON_NOISE_RATIO		= 24,
	DESCRIPTOR_PF_PITCH					= 25,
	DESCRIPTOR_PF_PITCH_CONFIDENCE		= 26,
	DESCRIPTOR_PF_INHARMONICITY			= 27,
	DESCRIPTOR_PF_ROUGHNESS				= 28,
	DESCRIPTOR_PF_SPECTRAL_PEAKS		= 29
};

enum PBDescriptorType
{
	DESCRIPTOR_PB_NONE				= 0,
	DESCRIPTOR_PB_DURATION			= 1,
	DESCRIPTOR_PB_SPECTRAL_PEAKS	= 2
	
};

// Statistics enums

enum StatisticsType
{
	STATS_NONE,
	STATS_MEAN,
	STATS_MEDIAN,
	STATS_TIME_CENTROID,
	STATS_STANDARD_DEV,
	STATS_RANGE,
	STATS_MAX,
	STATS_MAX_POS,
	STATS_MIN,
	STATS_MIN_POS,
	
	STATS_PEAK,
	STATS_PEAK_POS,
	STATS_TROUGH,
	STATS_TROUGH_POS,
	
	STATS_CROSSING_PEAK,
	STATS_CROSSING_PEAK_POS,
	STATS_CROSSING_ABOVE,
	STATS_CROSSINGS_ABOVE,
	
	STATS_CROSSING_TROUGH,
	STATS_CROSSING_TROUGH_POS,
	STATS_CROSSING_BELOW,
	STATS_CROSSINGS_BELOW,
	
	STATS_LONGEST_CROSS_ABOVE,
	STATS_LONGEST_CROSSINGS_ABOVE,
	STATS_LONGEST_CROSS_BELOW,
	STATS_LONGEST_CROSSINGS_BELOW,
	
	STATS_RATIO_ABOVE,
	STATS_RATIO_BELOW
} ;

// ThresholdType enums

enum ThresholdType
{
	THRESH_ABS,
	THRESH_PEAK_MUL,
	THRESH_PEAK_DB,
	THRESH_PEAK_ADD,
	THRESH_MEAN_MUL,
	THRESH_MEAN_DB,
	THRESH_MEAN_ADD
};

// window type enums

enum WindowType
{
	WIND_HANN,
	WIND_HAMMING,
	WIND_KAISER,
	WIND_TRIANGLE,
	WIND_BLACKMAN,
	WIND_BLACKMAN_62,
	WIND_BLACKMAN_70,
	WIND_BLACKMAN_74,
	WIND_BLACKMAN_92,
	WIND_BLACKMAN_HARRIS,
	WIND_FLAT_TOP,
	WIND_RECT
};

// Symbols

extern t_symbol *ps_energy;
extern t_symbol *ps_energy_ratio;
extern t_symbol *ps_rolloff;
extern t_symbol *ps_spectral_crest;
extern t_symbol *ps_flux;
extern t_symbol *ps_mkl;
extern t_symbol *ps_foote;
extern t_symbol *ps_abs;
extern t_symbol *ps_rms;
extern t_symbol *ps_peakamp;
extern t_symbol *ps_loudness;
extern t_symbol *ps_lin_centroid;
extern t_symbol *ps_lin_spread;
extern t_symbol *ps_lin_skewness;
extern t_symbol *ps_lin_kurtosis;
extern t_symbol *ps_log_centroid;
extern t_symbol *ps_log_spread;
extern t_symbol *ps_log_skewness;
extern t_symbol *ps_log_kurtosis;
extern t_symbol *ps_lin_brightness;
extern t_symbol *ps_log_brightness;
extern t_symbol *ps_sfm;
extern t_symbol *ps_noise_ratio;
extern t_symbol *ps_harmonic_ratio;
extern t_symbol *ps_pitch;
extern t_symbol *ps_confidence;
extern t_symbol *ps_inharmonicity;
extern t_symbol *ps_roughness;
extern t_symbol *ps_spectral_peaks;
extern t_symbol *ps_duration;


extern t_symbol *ps_mean;
extern t_symbol *ps_median;	
extern t_symbol *ps_time_centroid;	
extern t_symbol *ps_standard_dev;
extern t_symbol *ps_range;

extern t_symbol *ps_max;
extern t_symbol *ps_max_pos;
extern t_symbol *ps_min;
extern t_symbol *ps_min_pos;
extern t_symbol *ps_peak_pos;
extern t_symbol *ps_peak;
extern t_symbol *ps_trough;
extern t_symbol *ps_trough_pos;

extern t_symbol *ps_crossing_peak;
extern t_symbol *ps_crossing_peak_pos;
extern t_symbol *ps_cross_above;
extern t_symbol *ps_crossings_above;

extern t_symbol *ps_crossing_trough;
extern t_symbol *ps_crossing_trough_pos;
extern t_symbol *ps_cross_below;
extern t_symbol *ps_crossings_below;

extern t_symbol *ps_longest_cross_above;
extern t_symbol *ps_longest_cross_below;
extern t_symbol *ps_longest_crossings_above;
extern t_symbol *ps_longest_crossings_below;

extern t_symbol *ps_ratio_above;
extern t_symbol *ps_ratio_below;

extern t_symbol *ps_threshold;
extern t_symbol *ps_masktime;


extern t_symbol *ps_peak_mul;
extern t_symbol *ps_peak_add;
extern t_symbol *ps_peak_db;
extern t_symbol *ps_mean_mul;
extern t_symbol *ps_mean_add;
extern t_symbol *ps_mean_db;


extern t_symbol *ps_rectangle;
extern t_symbol *ps_hann;
extern t_symbol *ps_hamming;
extern t_symbol *ps_kaiser;
extern t_symbol *ps_triangle;
extern t_symbol *ps_blackman;
extern t_symbol *ps_blackman62;
extern t_symbol *ps_blackman70;
extern t_symbol *ps_blackman74;
extern t_symbol *ps_blackman92;
extern t_symbol *ps_blackman_harris;
extern t_symbol *ps_flattop;

extern t_symbol *ps_nullsym;


#endif /* _DESCRIPTORS_CONSTANTS_ */
