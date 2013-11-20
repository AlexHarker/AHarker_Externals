
/*
 *  descriptors_statistics.h
 *
 *	A header file for the statistical routines in the descriptors~ object.
 *	Given an array of raw per frame descriptor values these routines calculate various useful statisical values.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_STATS_
#define _DESCRIPTORS_STATS_

#include "descriptors_combsort.h"
#include "descriptors_constants.h"

// Common statistical values

double calc_mean_and_time_centroid (double *current_data, long num_frames, double *time_centroid_ret, double frame_to_ms_val);
double calc_standard_deviation (double *current_data, long num_frames, double mean);
double calc_median (double *current_data, long num_frames);

// N searches (search for n min / max / peak / trough values)

void n_search_mask (char *mask, long num_frames, long pos, long mask_size);
void calc_n_max (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double frame_to_ms_val);
void calc_n_min (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double frame_to_ms_val);
void calc_n_peak (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double frame_to_ms_val);
void calc_n_trough (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double frame_to_ms_val);

// Threshold searches (search for crossing above / below threshold

long threshold_search_mask (double *current_data, char *mask, long num_frames, long pos, long mask_size, char above_flag, double threshold, long *cross_point2_ret);
void calc_n_thresh_cross_above (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double *cross_pos1, double *cross_pos2, double threshold, double frame_to_ms_val);
void calc_n_thresh_cross_below (double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double *cross_pos1, double *cross_pos2, double threshold, double frame_to_ms_val);
void calc_longest_crossing_points_above (double *current_data, long num_frames, long mask_size, long N, double *n_val, double *cross1, double *cross2, double threshold, double frame_to_ms_val);
void calc_longest_crossing_points_below (double *current_data, long num_frames, long mask_size, long N, double *n_val, double *cross1, double *cross2, double threshold, double frame_to_ms_val);

// Threshold ratio calculation

double calc_threshold_ratio (double *current_data, long num_frames, char above_flag, double threshold);


#endif /* _DESCRIPTORS_STATS_ */
