
/*
 *  descriptors_set_descriptors.c
 *
 *	Routines for setting the descriptors to be calculated by the descriptors~ and descriptorsrt~ objects.
 *
 *	This code deals with translating the descriptors message arguments into a format that can be used by the calculation routines.
 *	This includes range checking arguments, and the substitution of absent arguments with defaults.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_set_descriptors.h"



// Matching for statistics and routine to handle parameters for statistics 



// Get the threshold type from a specifier (or use default if none specified)

enum ThresholdType match_threshold_type (t_atom **argv, short *argc)
{
	if (*argc)
	{
		t_symbol *symbolarg = atom_getsym(*argv);
		
		if (!symbolarg) 
			return THRESH_ABS;
		
		if (symbolarg == ps_abs)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_ABS;
		}
		if (symbolarg == ps_peak_mul)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_PEAK_MUL;
		}
		if (symbolarg == ps_peak_add)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_PEAK_ADD;
		}
		if (symbolarg == ps_peak_db)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_PEAK_DB;
		}
		if (symbolarg == ps_mean_mul) 
		{
			(*argv)++;
			(*argc)--;
			return THRESH_MEAN_MUL;
		}
		if (symbolarg == ps_mean_add)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_MEAN_ADD;
		}
		if (symbolarg == ps_mean_db)
		{
			(*argv)++;
			(*argc)--;
			return THRESH_MEAN_DB;
		}
	}
	
	return THRESH_ABS;
}


// Match a statistical type from a specifier, also setting masktime and threshold parameters

enum StatisticsType match_statistics (t_atom **argv, short *argc, double *mask_time, double *threshold, enum ThresholdType *thresh_type)
{
	if (*argc)
	{
		t_symbol *symbolarg = atom_getsym(*argv);
		
		if (!symbolarg) 
			return STATS_NONE;
		
		// Match a specfic statistical type (and return)
		
		if (symbolarg == ps_mean) return STATS_MEAN;
		if (symbolarg == ps_median) return STATS_MEDIAN;
		if (symbolarg == ps_time_centroid) return STATS_TIME_CENTROID;
		if (symbolarg == ps_standard_dev) return STATS_STANDARD_DEV;
		if (symbolarg == ps_range) return STATS_RANGE;
		if (symbolarg == ps_max) return STATS_MAX;
		if (symbolarg == ps_min) return STATS_MIN;
		if (symbolarg == ps_peak) return STATS_PEAK;
		if (symbolarg == ps_trough) return STATS_TROUGH;
		if (symbolarg == ps_max_pos) return STATS_MAX_POS;
		if (symbolarg == ps_min_pos) return STATS_MIN_POS;
		if (symbolarg == ps_peak_pos) return STATS_PEAK_POS;
		if (symbolarg == ps_trough_pos) return STATS_TROUGH_POS;
		if (symbolarg == ps_crossing_peak) return STATS_CROSSING_PEAK;
		if (symbolarg == ps_crossing_peak_pos) return STATS_CROSSING_PEAK_POS;
		if (symbolarg == ps_cross_above) return STATS_CROSSING_ABOVE;
		if (symbolarg == ps_crossings_above) return STATS_CROSSINGS_ABOVE;
		if (symbolarg == ps_crossing_trough) return STATS_CROSSING_TROUGH;
		if (symbolarg == ps_crossing_trough_pos) return STATS_CROSSING_TROUGH_POS;
		if (symbolarg == ps_cross_below) return STATS_CROSSING_BELOW;
		if (symbolarg == ps_crossings_below) return STATS_CROSSINGS_BELOW;
		if (symbolarg == ps_longest_crossings_above) return STATS_LONGEST_CROSSINGS_ABOVE;
		if (symbolarg == ps_longest_cross_above) return STATS_LONGEST_CROSS_ABOVE;
		if (symbolarg == ps_longest_crossings_below) return STATS_LONGEST_CROSSINGS_BELOW;
		if (symbolarg == ps_longest_cross_below) return STATS_LONGEST_CROSS_BELOW;
		if (symbolarg == ps_ratio_above) return STATS_RATIO_ABOVE;
		if (symbolarg == ps_ratio_below) return STATS_RATIO_BELOW;
		
		// Otherwise look to match either the masktime or threshold symbols
		
		if (symbolarg == ps_masktime)
		{
			// Do mask time (note that the last value will be the one actually used if more than one is specified)
			
			(*argv)++;
			(*argc)--;
			if (*argc)
			{
				if (atom_gettype(*argv) == A_SYM)
                    error("descriptors(rt)~: no mask time given");
				else	
				{
					*mask_time = atom_getfloat(*argv); 
					(*argv)++;
					(*argc)--;
				}
			}
			else 
			{
				error("descriptors(rt)~: no mask time given");
			}
			
			// Match the next statistical type
			
			return match_statistics(argv, argc, mask_time, threshold, thresh_type);
		}
		
		if (symbolarg == ps_threshold)
		{
			// Do threshold (note that the last value will be the one actually used if more than one is specified)
			
			(*argv)++;
			(*argc)--;
			if (*argc)
			{
				if (atom_gettype(*argv) == A_SYM)
                    error("descriptors(rt)~: no threshold given");
				else 
				{
					*threshold = atom_getfloat(*argv); 
					(*argv)++;
					(*argc)--;
					if (*argc && atom_gettype(*argv) == A_SYM)
						*thresh_type = match_threshold_type(argv, argc);
				}
			}
			else
			{
				error("descriptors(rt)~: no threshold given");
			}
			
			// Match the next statistical type

			return match_statistics (argv, argc, mask_time, threshold, thresh_type);
		}
	}

	// If we've got this far then we've not matched any statistical calculations
	
	return STATS_NONE;
}


// Handle arguments for statistical calculations

long descriptors_descriptors_statistics(long *pf_output_params, double *pf_calc_params, t_atom **argv, short *argc, long num_params_left, long num_descriptors, long output_pos, long *num_to_output)
{	
	enum ThresholdType thresh_type = THRESH_ABS;
	
	t_atom *argv_local = *argv;
	
	short argc_local = *argc;
	
	long *n_store_ptr = pf_output_params++;
	
	double threshold = 0.;
	double mask_time = 0;
	
	long final_num_to_output = 0;
	long total_num_output_params = 1;
	long output_pos_local = output_pos;
	long stat_num_output_params = 0;
	long num_to_output_local;
	long long_arg_temp;
	long n_store = 0;
	long descriptor_flags = 0;	
	long do_n_max = 0;
	long do_n_min = 0;
	long do_n_peak = 0;
	long do_n_trough = 0;
	long do_cross_above = 0;
	long do_cross_below = 0;
	long do_longest_cross_above = 0;
	long do_longest_cross_below = 0;
	
	// Get stats_type - if there is none then default to mean
	
	enum StatisticsType stats_type;

	stats_type = match_statistics(&argv_local, &argc_local, &mask_time, &threshold, &thresh_type);
	
	if (stats_type != STATS_NONE)
	{
		argv_local++;
		argc_local--;
	}
	else 
		stats_type = STATS_MEAN;
	
	do {
		
		num_to_output_local = 1;
		
		// Check we have space to store the next set of output parameters
		
		switch (stats_type)
		{
			case STATS_MEAN:
			case STATS_MEDIAN:
			case STATS_TIME_CENTROID:
			case STATS_STANDARD_DEV:
			case STATS_RANGE:
			case STATS_RATIO_ABOVE:
			case STATS_RATIO_BELOW:
				
				stat_num_output_params = 2;
				break;
				
			case STATS_MAX:
			case STATS_MIN:
			case STATS_PEAK:
			case STATS_TROUGH:
			case STATS_MAX_POS:
			case STATS_MIN_POS:
			case STATS_PEAK_POS:
			case STATS_TROUGH_POS:
			case STATS_CROSSING_PEAK:
			case STATS_CROSSING_PEAK_POS:
			case STATS_CROSSING_ABOVE:
			case STATS_CROSSINGS_ABOVE:
			case STATS_CROSSING_TROUGH:
			case STATS_CROSSING_TROUGH_POS:
			case STATS_CROSSING_BELOW:
			case STATS_CROSSINGS_BELOW:
			case STATS_LONGEST_CROSS_ABOVE:
			case STATS_LONGEST_CROSSINGS_ABOVE:
			case STATS_LONGEST_CROSS_BELOW:
			case STATS_LONGEST_CROSSINGS_BELOW:
				
				stat_num_output_params = 3;
				break;
				
			case STATS_NONE:
				
				stat_num_output_params = 0;
				break;
		}
		
		// If not bail
		
		if (num_params_left < stat_num_output_params)
		{
			error ("descriptors(rt)~: too many descriptors - bailing with %ld valid descriptors", num_descriptors);
			return 0;
		}
		
		// Store stats type and output position
		
		pf_output_params[0] = stats_type;
		pf_output_params[1] = output_pos_local;
		
		// Set flags and retrieve any additional parameters (using defaults if not present)
		
		switch (stats_type)
		{
			case STATS_MEAN:
				
				descriptor_flags |= DO_MEAN;
				break;
				
			case STATS_MEDIAN:
				
				descriptor_flags |= DO_MEDIAN;
				break;
				
			case STATS_TIME_CENTROID:
				
				descriptor_flags |= DO_MEAN;
				break;
				
			case STATS_STANDARD_DEV:
				
				descriptor_flags |= DO_MEAN;
				descriptor_flags |= DO_STDD;
				break;
				
			case STATS_RANGE:
				
				descriptor_flags |= DO_RANGE;
				if (!do_n_min) 
					do_n_min = 1;
				if (!do_n_max) 
					do_n_max = 1;
				break;
				
			case STATS_RATIO_ABOVE:
				
				descriptor_flags |= DO_RATIO_ABOVE;
				break;
				
			case STATS_RATIO_BELOW:
				
				descriptor_flags |= STATS_RATIO_BELOW;
				break;
				
			case STATS_MAX:
			case STATS_MAX_POS:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of max vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_n_max) 
					do_n_max = pf_output_params[2];
				
				break;
				
			case STATS_MIN:
			case STATS_MIN_POS:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of min vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_n_min) 
					do_n_min = pf_output_params[2];
				
				break;
				
			case STATS_PEAK:
			case STATS_PEAK_POS:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of peak vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_n_peak) 
					do_n_peak = pf_output_params[2];
				
				break;
				
			case STATS_TROUGH:
			case STATS_TROUGH_POS:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of trough vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_n_trough) 
					do_n_trough = pf_output_params[2];
				
				break;
				
			case STATS_CROSSING_PEAK:
			case STATS_CROSSING_PEAK_POS:
			case STATS_CROSSING_ABOVE:
			case STATS_CROSSINGS_ABOVE:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of crossing vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_cross_above) 
					do_cross_above = pf_output_params[2];
				if (stats_type == STATS_CROSSINGS_ABOVE) 
					num_to_output_local *= 2;
				
				break;
				
			case STATS_CROSSING_TROUGH:
			case STATS_CROSSING_TROUGH_POS:
			case STATS_CROSSING_BELOW:
			case STATS_CROSSINGS_BELOW:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of crossing vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_cross_below) 
					do_cross_below = pf_output_params[2];
				if (stats_type == STATS_CROSSINGS_BELOW) 
					num_to_output_local *= 2;
				
				break;
				
			case STATS_LONGEST_CROSS_ABOVE:
			case STATS_LONGEST_CROSSINGS_ABOVE:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of crossing vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_longest_cross_above) 
					do_longest_cross_above = pf_output_params[2];
				if (stats_type == STATS_LONGEST_CROSSINGS_ABOVE) 
					num_to_output_local *= 2;
				
				break;
				
			case STATS_LONGEST_CROSS_BELOW:
			case STATS_LONGEST_CROSSINGS_BELOW:
				
				pf_output_params[2] = num_to_output_local;
				if (argc_local && atom_gettype(argv_local) != A_SYM)
				{
					long_arg_temp = atom_getlong(argv_local++);
					argc_local--;
					if (long_arg_temp < 0) 
						error ("descriptors(rt)~: negative number of crossing vals - using %ld", pf_output_params[2]);
					else 
						pf_output_params[2] = num_to_output_local = long_arg_temp;
				}
				if (pf_output_params[2] > do_longest_cross_below) 
					do_longest_cross_below = pf_output_params[2];
				if (stats_type == STATS_LONGEST_CROSSINGS_BELOW) 
					num_to_output_local *= 2;
				
				break;
				
			case STATS_NONE:
				break;
		}
		
		// Update variables and pointers
		
		final_num_to_output += num_to_output_local;
		total_num_output_params += stat_num_output_params;
		output_pos_local += num_to_output_local;
		pf_output_params += stat_num_output_params;
		n_store++;
		
		// Check for the next calculation
		
		if (argc_local) 
			stats_type = match_statistics(&argv_local, &argc_local, &mask_time, &threshold, &thresh_type);
		else 
			stats_type = STATS_NONE; 
		
		if (stats_type == STATS_NONE) 
			break;
		
		argv_local++;
		argc_local--;
		
	} while (stats_type);
	
	*n_store_ptr = n_store;
	
	// Store pf_calc_params
	
	if (thresh_type == THRESH_MEAN_MUL || thresh_type == THRESH_MEAN_DB || thresh_type == THRESH_MEAN_ADD) 
		descriptor_flags |= DO_MEAN;
	if ((thresh_type == THRESH_PEAK_MUL || thresh_type == THRESH_PEAK_DB || thresh_type == THRESH_PEAK_ADD) && !do_n_max) 
		do_n_max = 1;
	
	pf_calc_params[0] = (double) descriptor_flags;	
	pf_calc_params[1] = mask_time;
	pf_calc_params[2] = (double) do_n_max;
	pf_calc_params[3] = (double) do_n_min;
	pf_calc_params[4] = (double) do_n_peak;
	pf_calc_params[5] = (double) do_n_trough;
	pf_calc_params[6] = (double) do_cross_above;
	pf_calc_params[7] = (double) do_cross_below;
	pf_calc_params[8] = (double) do_longest_cross_above;
	pf_calc_params[9] = (double) do_longest_cross_below;
	pf_calc_params[10] = threshold;
	pf_calc_params[11] = (double) thresh_type;
	
	*num_to_output = final_num_to_output;
	*argv = argv_local;
	*argc = argc_local;
	
	return total_num_output_params;
}



// Matching for descriptors 



// Match only per frame descriptors

enum PFDescriptorType match_pf_descriptor (t_atom *arg, char rt_flag)
{
	t_symbol *symbolarg = atom_getsym(arg);
	
	if (!symbolarg) 
		return DESCRIPTOR_PF_NONE;
	
	if (symbolarg == ps_energy) return DESCRIPTOR_PF_ENERGY;
	if (symbolarg == ps_energy_ratio) return DESCRIPTOR_PF_ENERGY_RATIO;
	if (symbolarg == ps_rolloff) return DESCRIPTOR_PF_SPECTRAL_ROLLOFF;
	if (symbolarg == ps_spectral_crest) return DESCRIPTOR_PF_SPECTRAL_CREST;
	if (symbolarg == ps_flux) return DESCRIPTOR_PF_FLUX;
	if (symbolarg == ps_mkl) return DESCRIPTOR_PF_MKL;
	if (symbolarg == ps_foote) return DESCRIPTOR_PF_FOOTE;
	if (symbolarg == ps_abs) return DESCRIPTOR_PF_AVERAGE_AMP_ABS;
	if (symbolarg == ps_rms) return DESCRIPTOR_PF_AVERAGE_AMP_RMS;
	if (symbolarg == ps_peakamp) return DESCRIPTOR_PF_PEAK_AMP;
	if (symbolarg == ps_loudness) return DESCRIPTOR_PF_LOUDNESS;
	if (symbolarg == ps_lin_centroid) return DESCRIPTOR_PF_CENTROID_LIN;
	if (symbolarg == ps_lin_spread) return DESCRIPTOR_PF_SPREAD_LIN;
	if (symbolarg == ps_lin_skewness) return DESCRIPTOR_PF_SKEWNESS_LIN;
	if (symbolarg == ps_lin_kurtosis) return DESCRIPTOR_PF_KURTOSIS_LIN;
	if (symbolarg == ps_log_centroid) return DESCRIPTOR_PF_CENTROID_LOG;
	if (symbolarg == ps_log_spread) return DESCRIPTOR_PF_SPREAD_LOG;
	if (symbolarg == ps_log_skewness) return DESCRIPTOR_PF_SKEWNESS_LOG;
	if (symbolarg == ps_log_kurtosis) return DESCRIPTOR_PF_KURTOSIS_LOG;
	if (symbolarg == ps_lin_brightness) return DESCRIPTOR_PF_BRIGHTNESS_LIN;
	if (symbolarg == ps_log_brightness) return DESCRIPTOR_PF_BRIGHTNESS_LOG;
	if (symbolarg == ps_sfm) return DESCRIPTOR_PF_SPECTRAL_FLATNESS;
	if (symbolarg == ps_noise_ratio) return DESCRIPTOR_PF_NOISE_RATIO;
	if (symbolarg == ps_harmonic_ratio) return DESCRIPTOR_PF_NON_NOISE_RATIO;
	if (symbolarg == ps_pitch) return DESCRIPTOR_PF_PITCH;
	if (symbolarg == ps_confidence) return DESCRIPTOR_PF_PITCH_CONFIDENCE;
	if (symbolarg == ps_inharmonicity) return DESCRIPTOR_PF_INHARMONICITY;
	if (symbolarg == ps_roughness) return DESCRIPTOR_PF_ROUGHNESS;
	if (symbolarg == ps_spectral_peaks && rt_flag) return DESCRIPTOR_PF_SPECTRAL_PEAKS;
	
	return DESCRIPTOR_PF_NONE;
}


// Match only per block descriptors


enum PBDescriptorType match_pb_descriptor (t_atom *arg, char rt_flag)
{
	t_symbol *symbolarg = atom_getsym(arg);
	
	if (!symbolarg) return DESCRIPTOR_PB_NONE;
	if (symbolarg == ps_duration) return DESCRIPTOR_PB_DURATION;
	if (symbolarg == ps_spectral_peaks && !rt_flag) return DESCRIPTOR_PB_SPECTRAL_PEAKS;
	
	return DESCRIPTOR_PB_NONE;
}


// Match either per frame or per block descriptors (N.B. return the per block descriptors as negative values)

long match_descriptor(t_atom *arg, char rt_flag)
{
	long descriptor_type = (long) match_pf_descriptor(arg, rt_flag);
	
	if (descriptor_type) 
		return descriptor_type;
	
	return -((long) match_pb_descriptor(arg, rt_flag));
}



// Routines for dealing with different types of descriptors 



long descriptors_descriptors_pb (enum PBDescriptorType descriptor_type, double *pb_params, t_atom **argv, short *argc, long num_params, long num_pb_descriptors, long *num_to_output)
{
	t_atom *argv_local = *argv;
	short argc_local = *argc;
	
	long descriptor_num_params = 0;
	long num_to_output_local = 1;
	
	double double_arg_temp;
		
	// Check we have space to store the parameters
		
	switch (descriptor_type)
	{
		case DESCRIPTOR_PB_SPECTRAL_PEAKS:
			descriptor_num_params = 3;
			break;
			
		case DESCRIPTOR_PB_DURATION: 
			descriptor_num_params = 1;
			break;
			
		case DESCRIPTOR_PB_NONE:
			descriptor_num_params = 0;
			break;
	}
	
	// If not then bail
	
	if ((num_params + descriptor_num_params) > MAX_PB_PARAMS)
	{
		error ("descriptors(rt)~: too many descriptors - bailing with %ld valid descriptors", num_pb_descriptors);
		return 0;
	}
	
	// Store descriptor type and then range check parameters / substitute defaults / store parameters
	
	pb_params[0] = (double) descriptor_type;
	
	switch (descriptor_type)
	{
		case DESCRIPTOR_PB_SPECTRAL_PEAKS:
	
			pb_params[1] = 10.;					// NPeaks
			pb_params[2] = 15.;					// median Span
								
			if (argc_local && atom_gettype(argv_local) != A_SYM)
			{
				double_arg_temp = (double) atom_getlong(argv_local++);
				argc_local--;
				if (double_arg_temp < 0) 
					error ("descriptors(rt)~: negative number of peaks - using %ld", (long) pb_params[1]);
				else 
					pb_params[1] = double_arg_temp;
			}
				
			if (argc_local && atom_gettype(argv_local) != A_SYM)
			{
				double_arg_temp = atom_getfloat(argv_local++);
				argc_local--;
				if (double_arg_temp < 0) 
					error ("descriptors(rt)~: negative median span - using %ld", (long) pb_params[2]);
				else 
					pb_params[2] = double_arg_temp;
			}
			
			num_to_output_local = 2 * (long) pb_params[1];
			break;
			
		case DESCRIPTOR_PB_DURATION:
		case DESCRIPTOR_PB_NONE:
		
			break;
	}
	
	// Store variables
	
	*num_to_output = num_to_output_local;
	*argv = argv_local;
	*argc = argc_local;
	
	return descriptor_num_params;
}


long descriptors_descriptors_pf (enum PFDescriptorType descriptor_type, double *pf_params, t_atom **argv, short *argc, long num_params, long num_pf_descriptors, long *num_to_output, char rt_flag)
{
	long descriptor_num_params = 0;
	long num_to_output_local = 1;
	double double_arg_temp;
		
	t_atom *argv_local = *argv;
	short argc_local = *argc;
	
	// Check we have space to store this parameter
		
	switch (descriptor_type)
	{
		case DESCRIPTOR_PF_MKL:
		
			descriptor_num_params = 8;
			break;
			
		case DESCRIPTOR_PF_FLUX:
		
			descriptor_num_params = 7;
			break;
			
		case DESCRIPTOR_PF_FOOTE:
			
			descriptor_num_params = 5;
			break;
			
		case DESCRIPTOR_PF_ENERGY: 
		case DESCRIPTOR_PF_SPECTRAL_CREST:
		case DESCRIPTOR_PF_INHARMONICITY:
		case DESCRIPTOR_PF_BRIGHTNESS_LIN: 
		case DESCRIPTOR_PF_BRIGHTNESS_LOG: 
		
			descriptor_num_params = 4;
			break;
			
		case DESCRIPTOR_PF_ENERGY_RATIO:
		case DESCRIPTOR_PF_CENTROID_LIN: 
		case DESCRIPTOR_PF_SPREAD_LIN:
		case DESCRIPTOR_PF_SKEWNESS_LIN:
		case DESCRIPTOR_PF_KURTOSIS_LIN:
		case DESCRIPTOR_PF_CENTROID_LOG: 
		case DESCRIPTOR_PF_SPREAD_LOG:
		case DESCRIPTOR_PF_SKEWNESS_LOG:
		case DESCRIPTOR_PF_KURTOSIS_LOG:
		case DESCRIPTOR_PF_SPECTRAL_FLATNESS:
		case DESCRIPTOR_PF_ROUGHNESS:
		case DESCRIPTOR_PF_SPECTRAL_PEAKS:
				
			descriptor_num_params = 3;
			break;
			
		case DESCRIPTOR_PF_SPECTRAL_ROLLOFF:
		case DESCRIPTOR_PF_AVERAGE_AMP_ABS:
		case DESCRIPTOR_PF_AVERAGE_AMP_RMS:
		case DESCRIPTOR_PF_PEAK_AMP:
		case DESCRIPTOR_PF_LOUDNESS:
		case DESCRIPTOR_PF_NOISE_RATIO:
		case DESCRIPTOR_PF_NON_NOISE_RATIO:
		case DESCRIPTOR_PF_PITCH:
		case DESCRIPTOR_PF_PITCH_CONFIDENCE:
		
			descriptor_num_params = 2;
			break;
				
		case DESCRIPTOR_PF_NONE:
			
			descriptor_num_params = 0;
				break;

	}
	
	// If not then bail
	
	if ((num_params + descriptor_num_params) > MAX_PF_PARAMS)
	{
		error ("descriptors(rt)~: too many descriptors - bailing with %ld valid descriptors", num_pf_descriptors);
		return 0;
	}
		
	// Store descriptor type and then range check parameters / substitute defaults / store parameters

	pf_params[0] = (double) descriptor_type;
	
	switch (descriptor_type)
	{
		case DESCRIPTOR_PF_ENERGY:
		case DESCRIPTOR_PF_ENERGY_RATIO:
		case DESCRIPTOR_PF_SPECTRAL_CREST:
		
			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			
			if (descriptor_type == DESCRIPTOR_PF_ENERGY || descriptor_type == DESCRIPTOR_PF_SPECTRAL_CREST) 
				pf_params[3] = 1.;				// db conversion flag
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (descriptor_type == DESCRIPTOR_PF_ENERGY || descriptor_type == DESCRIPTOR_PF_SPECTRAL_CREST)
			{					
				 if (!argc_local || atom_gettype(argv_local) == A_SYM) 
					 break;
				pf_params[3] = atom_getfloat(argv_local++);
				argc_local--;
			}
							
			break;
		
		
		
		case DESCRIPTOR_PF_FLUX:
		
			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			pf_params[3] = 1.;					// forward only flag
			pf_params[4] = 1.;					// square flag
			pf_params[5] = 1.;					// normalise spectrum flag
			pf_params[6] = 1.;					// frames to look back
						
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[3] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[4] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[5] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[6] = atom_getlong(argv_local++);
			argc_local--;
			if (pf_params[6] >= RING_BUFFER_SIZE || pf_params[6] < 1)
			{ 
				error ("descriptors(rt)~: frame lookback not within range - using %ld", RING_BUFFER_SIZE - 1);
				pf_params[6] = RING_BUFFER_SIZE - 1;
			}
			
			break;
		
		
				
		case DESCRIPTOR_PF_MKL:
		
			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			pf_params[3] = LOG_MIN;				// log threshold
			pf_params[4] = 1.;					// forward only flag
			pf_params[5] = 0.;					// weight with frame2 amplitudes
			pf_params[6] = 0.;					// normalise spectrum flag
			pf_params[7] = 1.;					// frames to look back
						
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			argc_local--;
			
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[3] = log(dbtoa(atom_getfloat(argv_local++)));
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[4] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[5] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[6] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[7] = atom_getlong(argv_local++);
			argc_local--;
			if (pf_params[7] >= RING_BUFFER_SIZE || pf_params[7] < 1)
			{ 
				error ("descriptors(rt)~: frame lookback not within range - using %ld", RING_BUFFER_SIZE - 1);
				pf_params[7] = RING_BUFFER_SIZE - 1;
			}
			
			break;

		
				
		case DESCRIPTOR_PF_FOOTE:
		
			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			pf_params[3] = 1.;					// forward only flag
			pf_params[4] = 1.;					// frames to look back
						
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[3] = atom_getfloat(argv_local++) != 0.;
			argc_local--;
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[4] = atom_getlong(argv_local++);
			argc_local--;
			if (pf_params[4] >= RING_BUFFER_SIZE || pf_params[4] < 1)
			{ 
				error ("descriptors(rt)~: frame lookback not within range - using %ld", RING_BUFFER_SIZE - 1);
				pf_params[3] = RING_BUFFER_SIZE - 1;
			}
			
			break;
				
		
		
		case DESCRIPTOR_PF_SPECTRAL_ROLLOFF:
							
			pf_params[1] = 0.95;				// threshold
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			double_arg_temp = atom_getfloat(argv_local++);
			argc_local--;
			if (double_arg_temp > 0. && double_arg_temp < 1.) 
				pf_params[1] = double_arg_temp;
			else error ("descriptors(rt)~: rolloff point should be between 0 and 1 - using %lf", pf_params[1]); 
			
			break;

		
		
		case DESCRIPTOR_PF_AVERAGE_AMP_ABS:
		case DESCRIPTOR_PF_AVERAGE_AMP_RMS:
		case DESCRIPTOR_PF_PEAK_AMP:
		case DESCRIPTOR_PF_LOUDNESS:
		
			pf_params[1] = 1.;					// db Conversion flag
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			
			break;
									
		
		
		case DESCRIPTOR_PF_CENTROID_LIN: 
		case DESCRIPTOR_PF_SPREAD_LIN:
		case DESCRIPTOR_PF_SKEWNESS_LIN:
		case DESCRIPTOR_PF_KURTOSIS_LIN:
		case DESCRIPTOR_PF_CENTROID_LOG: 
		case DESCRIPTOR_PF_SPREAD_LOG:
		case DESCRIPTOR_PF_SKEWNESS_LOG:
		case DESCRIPTOR_PF_KURTOSIS_LOG:
		case DESCRIPTOR_PF_SPECTRAL_FLATNESS:
				
			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
							
			break;
			
			
			
			case DESCRIPTOR_PF_BRIGHTNESS_LIN:
			case DESCRIPTOR_PF_BRIGHTNESS_LOG:

			pf_params[1] = 0.;					// min freq
			pf_params[2] = 192000.;				// max freq
			pf_params[3] = 0.68;				// threshold

			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[1] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[1] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			pf_params[2] = atom_getfloat(argv_local++);
			argc_local--;
			if (pf_params[2] < 0) 
				error ("descriptors(rt)~: negative frequency");
			
			if (!argc_local || atom_gettype(argv_local) == A_SYM) 
				break;
			double_arg_temp = atom_getfloat(argv_local++);
			argc_local--;	
			if (double_arg_temp > 0. && double_arg_temp <= 1.) 
				pf_params[3] = double_arg_temp;
			else 
				error("descriptors(rt)~: threshold should be between 0 and 1 - using %lf", pf_params[3]);
					
			break;

		


			case DESCRIPTOR_PF_NOISE_RATIO:
			case DESCRIPTOR_PF_NON_NOISE_RATIO:
		
				pf_params[1] = 15.;				// median span
			
				if (!argc_local || atom_gettype(argv_local) == A_SYM) 
					break;
				double_arg_temp = (double) atom_getlong(argv_local++);
				argc_local--;
				if (double_arg_temp < 0) 
					error("descriptors(rt)~: negative median span - using %ld", (long) pf_params[1]);
				else pf_params[1] = double_arg_temp;
									
				break;

			
			
			case DESCRIPTOR_PF_PITCH:
			case DESCRIPTOR_PF_PITCH_CONFIDENCE:
						
				pf_params[1] = 0.68;			// threshold
					
				if (!argc_local || atom_gettype(argv_local) == A_SYM) 
					break;
				double_arg_temp = atom_getfloat(argv_local++);
				argc_local--;	
				if (double_arg_temp > 0. && double_arg_temp <= 1.) 
					pf_params[1] = double_arg_temp;
				else 
					error("descriptors(rt)~: threshold should be between 0 and 1 - using %lf", pf_params[1]);

				break;
								
			
			
			case DESCRIPTOR_PF_INHARMONICITY:
			case DESCRIPTOR_PF_ROUGHNESS:
			case DESCRIPTOR_PF_SPECTRAL_PEAKS:
		
				pf_params[1] = 10.;				// n peaks
				pf_params[2] = 15.;				// median span
				
				if (descriptor_type == DESCRIPTOR_PF_INHARMONICITY)	
					pf_params[3] = 0.68;		// threshold
				if (descriptor_type == DESCRIPTOR_PF_SPECTRAL_PEAKS) 
					num_to_output_local = 2 * (long) pf_params[1];
					
				if (!argc_local || atom_gettype(argv_local) == A_SYM) 
					break;
				double_arg_temp = (double) atom_getlong(argv_local++);
				argc_local--;
				if (double_arg_temp < 0) 
					error("descriptors(rt)~: negative number of peaks - using %ld", (long) pf_params[1]);
				else pf_params[1] = double_arg_temp;
				
				if (descriptor_type == DESCRIPTOR_PF_SPECTRAL_PEAKS)
					num_to_output_local = 2 * (long) pf_params[1];
				
				if (!argc_local || atom_gettype(argv_local) == A_SYM) 
					break;
				double_arg_temp = (double) atom_getlong(argv_local++);
				argc_local--;
				if (double_arg_temp < 0) 
					error("descriptors(rt)~: negative median span - using %ld", (long) pf_params[2]);
				else pf_params[2] = double_arg_temp;
				
				if (descriptor_type == DESCRIPTOR_PF_INHARMONICITY)
				{
					 if (!argc_local || atom_gettype(argv_local) == A_SYM) 
						 break;
					double_arg_temp = atom_getfloat(argv_local++);
					argc_local--;	
					if (double_arg_temp > 0. && double_arg_temp <= 1.) 
						pf_params[3] = double_arg_temp;
					else 
						error("descriptors(rt)~: threshold should be between 0 and 1 - using %lf", pf_params[3]); 
				}
			
				break;
				
			case DESCRIPTOR_PF_NONE:
				break;
	}
	
	// Store variables
	
	*num_to_output = num_to_output_local;
	*argv = argv_local;
	*argc = argc_local;
	return descriptor_num_params;
}



// Main routines for setting the desciprotrs (by object) 



void descriptors_descriptors_non_rt (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{	
	long descriptor_type;
	long num_pf_descriptors = 0;
	long num_pb_descriptors = 0;
	long num_pf_params = 0;
	long num_pb_params = 0;
	long descriptor_num_params = 0;
	long output_pos = 0;
	long num_to_output;
	long stats_num_params;
	long output_params_left = MAX_PF_OUTPUT_PARAMS;
	
	double *pf_params = x->pf_params;
	double *pb_params = x->pb_params;
	double *pf_calc_params = x->pf_calc_params;
	
	long *pf_output_params = x->pf_output_params;
	long *pb_pos = x->pb_pos;
	
	x->do_sum_amps = 0;

	while (argc)
	{
		// Get descriptor type
		
		descriptor_type = match_descriptor(argv++, 0);
		argc--;
		
		// Bail if no descriptor is matched
		
		if (descriptor_type == DESCRIPTOR_PF_NONE)
		{
			error ("descriptors(rt)~: unknown descriptor / incorrectly formatted descriptors - bailing with %ld valid descriptors", num_pf_descriptors + num_pb_descriptors);
			break;
		}
		
		if (descriptor_type < 0)
		{
			// Per block descriptors
			
			// Parse the descriptor arguments (and store into params)

			descriptor_num_params = descriptors_descriptors_pb((enum PBDescriptorType)-descriptor_type, pb_params, &argv, &argc, num_pb_params, num_pb_descriptors, &num_to_output);
			
			if (descriptor_num_params)
			{
				// Update variables and pointers

				pb_params += descriptor_num_params;
				num_pb_params += descriptor_num_params;
				pb_pos[num_pb_descriptors] = output_pos;
				output_pos += num_to_output;
				
				// Bail if we will have too long an output

				if (output_pos > MAX_OUTPUT)
				{
					error ("descriptors(rt)~: too many output values - bailing with %ld valid descriptors", num_pf_descriptors + num_pb_descriptors);
					break;
				}
				
				if (-descriptor_type == DESCRIPTOR_PB_SPECTRAL_PEAKS) x
					->do_sum_amps = 1;
				
				num_pb_descriptors++;
			}
			else break;
		}
		else
		{
			// Per frame descriptors
			
			// Parse the descriptor arguments (and store into params)
			
			descriptor_num_params = descriptors_descriptors_pf((enum PFDescriptorType) descriptor_type, pf_params, &argv, &argc, num_pf_params, num_pf_descriptors, &num_to_output, 0);		// Note We Ignore the num_to_output return here
			
			if (descriptor_num_params)
			{
				// Update variables and pointers

				pf_params += descriptor_num_params;
				num_pf_params += descriptor_num_params;
				
				// Parse statistical arguments
				
				stats_num_params = descriptors_descriptors_statistics(pf_output_params, pf_calc_params, &argv, &argc, output_params_left, num_pf_descriptors + num_pb_descriptors, output_pos, &num_to_output);
				
				// Update variables and pointers
				
				output_params_left -= stats_num_params;
				pf_output_params += stats_num_params;
				output_pos += num_to_output;
				pf_calc_params += 12;
				
				// Bail if we will have too long an output
				
				if (output_pos > MAX_OUTPUT)
				{
					error ("descriptors(rt)~: too many output values - bailing with %ld valid descriptors", num_pf_descriptors + num_pb_descriptors);
					break;
				}
				
				num_pf_descriptors++;
			}
			else break;
		}
	}
	
	// Store variables
	
	x->num_pb_descriptors = num_pb_descriptors;
	x->num_pf_descriptors = num_pf_descriptors;
	x->output_length = output_pos;
	if (x->descriptor_feedback)
		post ("descriptors(rt)~: set %ld descriptors", num_pf_descriptors + num_pb_descriptors);
}


void descriptors_descriptors_rt(t_descriptors *x, t_symbol *msg, short argc, t_atom *argv)
{
	long descriptor_type;
	long descriptor_num_params = 0;
	long num_pf_descriptors = 0;													
	long num_params = 0;
	long output_pos = 0;
	long num_to_output;

	double *pf_params = x->pf_params;
	
	while (argc)
	{
		// Get descriptor type
		
		descriptor_type = match_descriptor(argv++, 1);
		argc--;
		
		// Bail if no descriptor matched
		
		if (descriptor_type == DESCRIPTOR_PF_NONE)
		{
			error ("descriptors(rt)~: unknown descriptor / incorrectly formatted descriptors - bailing with %ld valid descriptors", num_pf_descriptors);
			break;
		}
		
		// Bail if the descriptor is only for non-realtime use (ie is a per block descriptor)
		
		if (descriptor_type < 0)
		{
			error ("descriptors(rt)~: descriptor available for non real-time only - bailing with %ld valid descriptors", num_pf_descriptors);
			break;
		}
		
		// Parse the descriptor arguments (and store into params)
		
		descriptor_num_params = descriptors_descriptors_pf((enum PFDescriptorType) descriptor_type, pf_params, &argv, &argc, num_params, num_pf_descriptors, &num_to_output, 1);
		
		if (descriptor_num_params)
		{
			// Update variables and pointers
			
			pf_params += descriptor_num_params;
			num_params += descriptor_num_params;
			output_pos += num_to_output;
			
			// Bail if we will have too long an output
			
			if (output_pos > MAX_OUTPUT)
			{
				error ("descriptors(rt)~: too many output values - bailing with %ld valid descriptors", num_pf_descriptors);
				break;
			}
			
			num_pf_descriptors++;
		}
		else 
		{
			break;
		}
	}
	
	// Store variables
	
	x->num_pf_descriptors = num_pf_descriptors;
	x->output_length = output_pos;
	if (x->descriptor_feedback)
		post ("descriptors(rt)~: set %ld descriptors", num_pf_descriptors);
}



		
