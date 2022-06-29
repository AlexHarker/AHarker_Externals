
/*
 *  descriptors_statistics.c
 *
 *	Statistical routines for the descriptors~ object.
 *	Given an array of raw per frame descriptor values these routines calculate various useful statisical values.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include "descriptors_statistics.h"
#include "descriptors_sort.hpp"



// Common Stats - mean, Time Centroid, Standard Deviation and Median 



double calc_mean_and_time_centroid(double *current_data, long num_frames, double *time_centroid_ret, double frame_to_ms_val)
{
	// The time centroid is the average of the time values weighted by value
	// This will return the time around which the data is weighted most highly
	
	double current_val;
	double centroid_sum = 0;
	double sum = 0;
	long num_valid = 0;
	long i;

	for (i = 0; i < num_frames; i++)
	{
		current_val = current_data[i];
		if (current_val != DBL_MAX)
		{	
			sum += current_val;
			centroid_sum += (double) i * current_val;
			num_valid++;
		}
	}	

	if (sum) 
		centroid_sum = (centroid_sum / sum)  * frame_to_ms_val;
	else 
		centroid_sum = DBL_MAX;

	*time_centroid_ret = centroid_sum;

	if (num_valid) 
		return sum / (double) num_valid;
	else 
		return sum = DBL_MAX;
}


double calc_standard_deviation(double *current_data, long num_frames, double mean)
{
	double current_val;
	double sum = 0;
	long num_valid = 0;
	long i;

	for (i = 0; i < num_frames; i++)
	{
		current_val = current_data[i];
		if (current_val != DBL_MAX)
		{
			current_val -= mean;
			current_val *= current_val;
			sum += current_val;
			num_valid++;
		}
	}

	if (num_valid) 
		return sqrt (sum / (double) num_valid);
	else 
		return DBL_MAX;
}


double calc_median(double *current_data, long num_frames)
{
	long i;
	
	// Sort data
	
	sort(current_data, num_frames);

	// Pick the value that is at the halfway point through the *valid* results
	
	for (i = num_frames - 1; i >= 0 ; i--)
		if (current_data[i] < DBL_MAX) 
			break;
		
	if (i == -1) 
		return DBL_MAX;
	else 
		return (current_data[i >> 1]);
}



// N Searches - min, max, Peak and Trough Stats 



// This routine spplies a mask based on the last found position for any of the below n value searches

void n_search_mask(char *mask, long num_frames, long pos, long mask_size)
{
	long from = pos - mask_size;
	long to = pos + mask_size + 1;
	long i;

	if (from < 0) 
		from = 0;
	if (to > num_frames) 
		to = num_frames;
	
	for (i = from; i < to; i++)
		mask[i] = 1;
}


// Find the N maximum values, with a mask preventing close toegther values from being chosen

void calc_n_max(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double frame_to_ms_val)
{
	double current_val;
	double max = -DBL_MAX;
	long max_pos;
	long i, j;
	
	// Zero the mask
	
	for (i = 0; i < num_frames; i++)
		mask[i] = 0;
	
	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid maximum
		
		max = -DBL_MAX;
		max_pos = -1;
		
		// Find the next maximum
		
		for (i = 0; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val > max && !mask[i] && current_val != DBL_MAX)
			{
				max = current_val;
				max_pos = i;
			}
		}
		
		// If max_pos is invalid then we are done
		
		if (max_pos == -1) 
			break;
		
		// Apply the mask and store the found values
		
		n_search_mask(mask, num_frames, max_pos, mask_size);
		n_max[j] = max;
		n_max_pos[j] = max_pos * frame_to_ms_val;
	}
	
	// Store DBL_MAX if there aren't any more values to be found
	
	for (; j < N; j++)
	{
		n_max[j] = DBL_MAX;
		n_max_pos[j] = DBL_MAX;
	}
}			


// Find the N minimum values, with a mask preventing close toegther values from being chosen

void calc_n_min(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double frame_to_ms_val)
{
	double current_val;
	double min = DBL_MAX;
	long min_pos;
	long i, j;
	
	// Zero the mask
	
	for (i = 0; i < num_frames; i++)
		mask[i] = 0;

	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid minimum

		min = DBL_MAX;
		min_pos = -1;
		
		for (i = 0; i < num_frames; i++)
		{
			// Find the next minimum

			current_val = current_data[i];
			if (current_val < min && !mask[i])
			{
				min = current_val;
				min_pos = i;
			}
		}
		
		// If min_pos is invalid then we are done

		if (min_pos == -1) 
			break;

		// Apply the mask and store the found values

		n_search_mask(mask, num_frames, min_pos, mask_size);
		n_min[j] = min;
		n_min_pos[j] = min_pos  * frame_to_ms_val;
	}
	
	
	// Store DBL_MAX if there aren't any more values to be found
	
	for (; j < N; j++)
	{
		n_min[j] = DBL_MAX;
		n_min_pos[j] = DBL_MAX;
	}
}		
	

// Find the N peak values (values that are larger than the two surrounding values), with a mask preventing close toegther values from being chosen

void calc_n_peak(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double frame_to_ms_val)
{
	double current_val;
	double prev_val;
	double next_val;
	double new_max;
	double max; 
	long max_pos;
	long new_max_pos;
	long i, j;
	
	// Zero the mask
	
	for (i = 0; i < num_frames; i++)
		mask[i] = 0;
		
	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid peak

		max = -DBL_MAX;
		max_pos = -1;
		
		current_val = -DBL_MAX;
		next_val = current_data[0];
		
		// Search for the next largest peak value
		
		for (i = 0; i < num_frames; i++)		
		{
			// Pass values through

			prev_val = current_val;
			current_val = next_val;
			next_val = current_data[i + 1];
			
			// See if the current value is a larger than the previous one and not masked
			
			if (current_val > max && current_val > prev_val && !mask[i] && current_val != DBL_MAX)
			{
				new_max = current_val;
				new_max_pos = i;
				
				// Now search right to find if this is a peak value
				
				for (; i < num_frames - 1; i++)
				{
					next_val = current_data[i + 1];
					
					// If the next value is not equal then break (we know if it is a peak or not)

					if (current_val > next_val)
					{
						max = new_max;
						max_pos = new_max_pos;
						break;
					}	
										
					if (current_val < next_val)
						break;
					
					prev_val = current_val;
					current_val = next_val;
				}
				
				if (current_val >= prev_val)
				{
					max = new_max;
					max_pos = new_max_pos;
				}
			}
		}
		
		// If max_pos is invalid then we are done

		if (max_pos == -1) 
			break;

		// Apply the mask and store the found values

		n_search_mask(mask, num_frames, max_pos, mask_size);
		n_max[j] = max;
		n_max_pos[j] = max_pos * frame_to_ms_val;
	}
	
	
	// Store DBL_MAX if there aren't any more values to be found
	
	for (; j < N; j++)
	{
		n_max[j] = DBL_MAX;
		n_max_pos[j] = DBL_MAX;
	}
}			


// Find the N trough values (values that are smaller than the two surrounding values), with a mask preventing close toegther values from being chosen

void calc_n_trough(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double frame_to_ms_val)
{
	double current_val, prev_val, next_val, min, new_min;
	long min_pos, new_min_pos;
	long i, j;
	
	// Zero the mask

	for (i = 0; i < num_frames; i++)
		mask[i] = 0;
		
	// Search for the next smallest trough value

	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid trough

		min = DBL_MAX;
		min_pos = -1;
		
		current_val = DBL_MAX;
		next_val = current_data[0];
		
		// Search for a nex smallest trough
		
		for (i = 0; i < num_frames; i++)		
		{
			// Pass values through

			prev_val = current_val;
			current_val = next_val;
			next_val = current_data[i + 1];
			
			// See if the current value is a smaller than the previous one and not masked

			if (current_val > min && current_val < prev_val && !mask[i] && current_val != DBL_MAX)
			{
				new_min = current_val;
				new_min_pos = i;
			
				// Now search right to find if this is a trough value
				
				for (; i < num_frames - 1; i++)
				{
					next_val = current_data[i + 1];
					
					// If the next value is not equal then break (we know if it is a trough or not)
					
					if (current_val < next_val)
					{
						min = new_min;
						min_pos = new_min_pos;
						break;
					}	
					
					if (current_val > next_val)
						break;
					
					prev_val = current_val;
					current_val = next_val;
				}
				
				if (current_val <= prev_val)
				{
					min = new_min;
					min_pos = new_min_pos;
				}
			}
		}

		// If min_pos is invalid then we are done

		if (min_pos == -1) 
			break;

		// Apply the mask and store the found values

		n_search_mask(mask, num_frames, min_pos, mask_size);
		n_min[j] = min;
		n_min_pos[j] = min_pos * frame_to_ms_val;
	}
	
	// Store DBL_MAX if there aren't any more values to be found
	
	for (; j < N; j++)
	{
		n_min[j] = DBL_MAX;
		n_min_pos[j] = DBL_MAX;
	}
}	



// threshold Crossing Searches 



// This routine spplies a mask based on the last found position for any of the below threshold searches, as well as returning the two crossing points

long threshold_search_mask(double *current_data, char *mask, long num_frames, long peak_pos, long mask_size, char above_flag, double threshold, long *cross_point2_ret)
{
	double current_val;
	long from = peak_pos - mask_size;
	long to = peak_pos + mask_size + 1;
	long cross_point1, cross_point2;
	long i;
	
	if (from < 0) 
		from = 0;
	if (to > num_frames) 
		to = num_frames;
	
	if (above_flag)											
	{
		// Peak routine
		
		// Search left (earlier) for cross point
		
		for (i = peak_pos; i > 0 ; i--)						
			if (current_data[i] < threshold) 
				break;
		cross_point1 = i;
			
		// Search right (later) for cross point
		
		for (i = peak_pos + 1; i < num_frames; i++)			
			if (current_data[i] < threshold) 
				break;
		cross_point2 = i;
	}
	else
	{
		// Trough routine
		
		// Search left (earlier) for cross point

		for (i = peak_pos; i > 0 ; i--)						
		{
			current_val = current_data[i - 1];
			if (current_val > threshold && current_val != DBL_MAX) 
				break;
		}
		cross_point1 = i;
			
		// Search right (later) for cross point

		for (i = peak_pos + 1; i < num_frames; i++)			
		{
			current_val = current_data[i - 1];
			if (current_val > threshold && current_val != DBL_MAX) 
				break;
		}		cross_point2 = i;
	}
	
	// Update mask points and do masking
	
	if (cross_point1 < from) 
		from = cross_point1;
	if (cross_point2 > to) 
		to = cross_point2;
	
	for (i = from; i < to; i++)
		mask[i] = 1;
		
	*cross_point2_ret = cross_point2;
	
	return cross_point1;
}


// Find the N peak values and crossing points that cross above the specified threshold (with masking as above)

void calc_n_thresh_cross_above(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_max, double *n_max_pos, double *cross_pos1, double *cross_pos2, double threshold, double frame_to_ms_val)
{
	double current_val, prev_val, next_val, max, new_max;
	long cross_point1, cross_point2;
	long max_pos, new_max_pos;
	long i, j;
	
	// Zero the mask
	
	for (i = 0; i < num_frames; i++)
		mask[i] = 0;
		
	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid value
		
		max = threshold;
		max_pos = -1;
		
		current_val = -DBL_MAX;
		next_val = current_data[0];
		
		// Search for the next largest peak value
		
		for (i = 0; i < num_frames; i++)		
		{
			// Pass values through
			
			prev_val = current_val;
			current_val = next_val;
			next_val = current_data[i + 1];
			
			if (current_val > max && current_val > prev_val && !mask[i] && current_val != DBL_MAX)
			{
				new_max = current_val;
				new_max_pos = i;
				
				// Now search right to see if it is a peak value
				
				for (; i < num_frames - 1; i++)
				{
					next_val = current_data[i + 1];
					
					// If the next value is not equal then break (we know if it is a peak or not)
					
					if (current_val > next_val)
					{
						max = new_max;
						max_pos = new_max_pos;
						break;
					}	
					
					if (current_val < next_val)
						break;
					
					prev_val = current_val;
					current_val = next_val;
				}
				
				if (current_val >= prev_val)
				{
					max = new_max;
					max_pos = new_max_pos;
				}
			}
		}
		
		// If max_pos is invalid then we are done
		
		if (max_pos == -1) 
			break;

		// Do msking and store values
		
		cross_point1 = threshold_search_mask(current_data, mask, num_frames, max_pos, mask_size, 1, threshold, &cross_point2);
		n_max[j] = max;
		n_max_pos[j] = max_pos * frame_to_ms_val;
		cross_pos1[j] = cross_point1 * frame_to_ms_val;
		cross_pos2[j] = cross_point2 * frame_to_ms_val;
	}
	
	// Store DBL_MAX if there aren't any more values to be found

	for (; j < N; j++)
	{
		n_max[j] = DBL_MAX;
		n_max_pos[j] = DBL_MAX;
		cross_pos1[j] = DBL_MAX;
		cross_pos2[j] = DBL_MAX;
	}
}			


// Find the N trough values and crossing points that cross below the specified threshold (with masking as above)

void calc_n_thresh_cross_below(double *current_data, long num_frames, long mask_size, char *mask, long N, double *n_min, double *n_min_pos, double *cross_pos1, double *cross_pos2, double threshold, double frame_to_ms_val)
{
	double current_val, prev_val, next_val, min, new_min;
	long cross_point1, cross_point2;
	long min_pos, new_min_pos;
	long i, j;	
	
	for (i = 0; i < num_frames; i++)
		mask[i] = 0;
		
	for (j = 0; j < N; j++)
	{
		// Assume there is not another valid value

		min = threshold;
		min_pos = -1;
		
		current_val = DBL_MAX;
		next_val = current_data[0];
		
		// Search for the next smallest trough value

		for (i = 0; i < num_frames; i++)		
		{
			// Pass values through

			prev_val = current_val;
			current_val = next_val;
			next_val = current_data[i + 1];
			
			// Search for a new in value smaller than the previous one
			
			for (i = 0; i < num_frames; i++)		
			{
				prev_val = current_val;
				current_val = next_val;
				next_val = current_data[i + 1];
				
				if (current_val > min && current_val < prev_val && !mask[i] && current_val != DBL_MAX)
				{
					new_min = current_val;
					new_min_pos = i;
					
					// Now search right to see if it is a trough value
					
					for (; i < num_frames - 1; i++)
					{
						next_val = current_data[i + 1];
						
						// If the next value is not equal then break (we know if it is a trough or not)
						
						if (current_val < next_val)
						{
							min = new_min;
							min_pos = new_min_pos;
							break;
						}	
						
						if (current_val > next_val)
							break;
						
						prev_val = current_val;
						current_val = next_val;
					}
					
					if (current_val <= prev_val)
					{
						min = new_min;
						min_pos = new_min_pos;
					}
				}
			}
		}
			
		// If min_pos is invalid then we are done
		
		if (min_pos == -1) 
			break;
		
		// Do msking and store values

		cross_point1 = threshold_search_mask(current_data, mask, num_frames, min_pos, mask_size, 0, threshold, &cross_point2);
		n_min[j] = min;
		n_min_pos[j] = min_pos * frame_to_ms_val;
		cross_pos1[j] = cross_point1 * frame_to_ms_val;
		cross_pos2[j] = cross_point2 * frame_to_ms_val;
	}
	
	// Store DBL_MAX if there aren't any more values to be found
	
	for (; j < N; j++)
	{
		n_min[j] = DBL_MAX;
		n_min_pos[j] = DBL_MAX;
		cross_pos1[j] = DBL_MAX;
		cross_pos2[j] = DBL_MAX;
	}
}	



// Crossing Points 



// Find the longest N periods that cross above the specified threshold (with masking as above)

void calc_longest_crossing_points_above(double *current_data, long num_frames, long mask_size, long N, double *n_val, double *cross1, double *cross2, double threshold, double frame_to_ms_val)
{
	double current_val;
	double length;
	long cross_point1;
	long cross_point2;
	long insert_point;
	long num_found = 0;
	long i = 0;
	long j;
	
	while (i < num_frames)
	{
		// Find the next crossing above the threshold 
		
		for (; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val > threshold && current_val != DBL_MAX) 
				break;
		}
		cross_point1 = i;
		
		// If we've reached the end without finding a crossing point then we are done
		
		if (i == num_frames)
			break;
			
		// Jump by the mask size (we act as though the value has stayed above the threshold until this point
		// Now find out when the value crosses back under the threshold
		
		i += mask_size;
		for (; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val < threshold) 
				break;
		}
		cross_point2 = i;
		
		// If this is longer than any of the stored values - insert it into the list, losing the lowest value
		
		length = (cross_point2 - cross_point1) * frame_to_ms_val; 
		
		for (j = 0; j < num_found; j++)
			if (length > n_val[j]) 
				break;
		insert_point = j;
		
		if (++num_found > N) 
			num_found = N;
		
		if (insert_point < num_found)
		{
			// Do swaps
			
			for (j = num_found - 1; j >= insert_point + 1; j--)
			{
				n_val[j] = n_val[j - 1];
				cross1[j] = cross1[j - 1];
				cross2[j] = cross2[j - 1];
			}
			
			// Store these values 
			
			n_val[insert_point] = length;
			cross1[insert_point] = cross_point1 * frame_to_ms_val;
			cross2[insert_point] = cross_point2 * frame_to_ms_val;
		}
	}
		
	// Store DBL_MAX if there aren't any more values to be found
	
	for (i = num_found; i < N; i++)
	{
		n_val[i] = DBL_MAX;
		cross1[i] = DBL_MAX;
		cross2[i] = DBL_MAX;
	}
}	


// Find the longest N periods that cross below the specified threshold (with masking as above)

void calc_longest_crossing_points_below(double *current_data, long num_frames, long mask_size, long N, double *n_val, double *cross1, double *cross2, double threshold, double frame_to_ms_val)
{
	double current_val;
	double length;
	long cross_point1;
	long cross_point2;
	long insert_point;
	long num_found = 0;
	long i = 0;
	long j;
			
	while (i < num_frames)
	{
		// Find the next crossing below the threshold 

		for (; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val < threshold) 
				break;
		}
		cross_point1 = i;
		
		// If we've reached the end without finding a crossing point then we are done

		if (i == num_frames)
			break;
		
		// Jump by the mask size (we act as though the value has stayed below the threshold until this point
		// Now find out when the value crosses back above the threshold
		
		i += mask_size;
		for (; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val > threshold && current_val != DBL_MAX) 
				break;
		}
		cross_point2 = i;
		
		// If this is longer than any of the stored values - insert it into the list, losing the lowest value
				
		length = ((cross_point2 - cross_point1) * frame_to_ms_val);									
		
		for (j = 0; j < num_found; j++)
			if (length > n_val[j]) 
				break;
		insert_point = j;
		
		if (++num_found > N) 
			num_found = N;
		
		if (insert_point < num_found)
		{
			// Do swaps
			
			for (j = num_found - 1; j >= insert_point + 1; j--)
			{
				n_val[j] = n_val[j - 1];
				cross1[j] = cross1[j - 1];
				cross2[j] = cross2[j - 1];
			}																	
			
			// Store these values 

			n_val[insert_point] = length;
			cross1[insert_point] = cross_point1 * frame_to_ms_val;
			cross2[insert_point] = cross_point2 * frame_to_ms_val;
		}
	}
		
	// Store DBL_MAX if there aren't any more values to be found
	
	for (i = num_found; i < N; i++)
	{
		n_val[i] = DBL_MAX;
		cross1[i] = DBL_MAX;
		cross2[i] = DBL_MAX;
	}
}	



// threshold Ratio 



// This routine calculates the ratio of values above, or below the specified threshold

double calc_threshold_ratio(double *current_data, long num_frames, char above_flag, double threshold)
{
	double current_val;
	long num_crossed = 0;
	long num_valid = 0;
	long i;
	
	if (above_flag)
	{
		// Above
		
		for (i = 0; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val != DBL_MAX)
			{
				num_valid++;
				if (current_val > threshold) 
					num_crossed++;
			}
		}
	}
	else
	{
		// Below
		
		for (i = 0; i < num_frames; i++)
		{
			current_val = current_data[i];
			if (current_val != DBL_MAX)
			{
				num_valid++;
				if (current_val < threshold) 
					num_crossed++;
			}
		}
	}
	
	if (num_valid) 
		return (double) num_crossed / (double) num_valid;
	
	return DBL_MAX;
}	


