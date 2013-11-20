
/*
 *  descriptors_set_descriptors.h
 *
 *	A header file for descriptors_set_descriptors.c for the descriptors~ and descriptorsrt~ objects.
 *
 *	This code deals with translating the descriptors message arguments into a format that can be used by the calculation routines.
 *	This includes range checking arguments, and the substitution of absent arguments with defaults.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#ifndef _DESCRIPTORS_SET_DESCRIPTORS_
#define _DESCRIPTORS_SET_DESCRIPTORS_


#include "descriptors_object_structure.h"
#include "descriptors_constants.h"
#include "descriptors_conversion_helpers.h"


// Matching for statistics and routine to handle parameters for statistics

enum ThresholdType match_threshold_type (t_atom **argv, short *argc);
enum StatisticsType match_statistics (t_atom **argv, short *argc, double *mask_time, double *threshold, enum ThresholdType *thresh_type);

// Routine for dealing with statistics

long descriptors_descriptors_statistics (long *pf_output_params, double *pf_calc_params, t_atom **argv, short *argc, long num_params_left, long num_descriptors, long output_pos, long *num_to_output);

// Matching for descriptors

enum PBDescriptorType match_pb_descriptor (t_atom *arg, char rt_flag);
enum PFDescriptorType match_pf_descriptor (t_atom *arg, char rt_flag);
long match_descriptor (t_atom *arg, char rt_flag);

// Routines for dealing with different types of descriptors

long descriptors_descriptors_pb (enum PBDescriptorType descriptor_type, double *pb_params, t_atom **argv, short *argc, long num_params, long num_pb_descriptors, long *num_to_output);
long descriptors_descriptors_pf (enum PFDescriptorType descriptor_type, double *pf_params, t_atom **argv, short *argc, long num_params, long num_pf_descriptors, long *num_to_output, char rt_flag);

// Main routines for setting the desciprotrs (by object)

void descriptors_descriptors_rt (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);
void descriptors_descriptors_non_rt (t_descriptors *x, t_symbol *msg, short argc, t_atom *argv);


#endif _DESCRIPTORS_SET_DESCRIPTORS_
