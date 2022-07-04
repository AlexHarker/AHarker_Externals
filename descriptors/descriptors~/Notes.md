** Issues **

General and Small

* Padding issue (not yet fully investigated)
* Old non RT object didn't respond to different buffer sample rates correctly (need to reset fft params) [now corrected]
* Precision etc. - some small differences in various places [note only]
* Shape desciptors (crest/sfm/skewness/kurtosis) - some differences for large fft with sine input [note only]

* -- Descriptors --

* Rolloff *now* has interpolation (so reports slightly lower) - N.B. it uses power (as Peeters / unlike flucoma default)
* SFM *was* using power rather than amplitudes [now corrected]
* Spectral crest *was* using power rather than amplitudes [now corrected]

* Linear spread *was* squaring the raw value before the Hz adjustment (so not a useful value) [now corrected]
* Higher spectral shapes (skewness + kurtosis) *were* wrong in early versions (lack of division by the overall amplitude sum) [now corrected]
* Log higher spectral shapes *were* still wrong in more recent versions [now corrected]

* Pitch module *had* a significant issue where pitch reports as 0 when invalid / there is a precision effect [now corrected]
* Pitch *now* reports as inf for no pitch (not zero) solving averaging and stats errors [now corrected]
* Pitch *was* based on an FFT with (possibly unimportant) time aliasing [now corrected]
* Pitch *was* faulty if the window size didn't match the FFT size [now corrected]

* Flux *had* a fixed index bug in the code producing consistently incorrect results [now corrected]
* Foote *did* return inf for the change between two zero frames (now returns zero which seems more correct) [now corrected]
* MKL *did* have some normalisation bugs [now corrected]
* MKL *did* return inf for the change between two zero frames (now returns zero which seems more correct) [now corrected]

* Noise ratio (and harmonic ratio) *did* have errors in the median filter and the bin indexing, meaning fairly meaningless results [now corrected]

* Roughness *did not* correctly include the hack for min amp (original code no longer available but may be on old hard disk)

* RT spectral_peaks reports in linear amps but non RT in db (with no options)
* Summary spectral peak finding returns zeros if it can't find enough peaks, not infs
* Median filtering implementation *was* broken [now corrected]
* All content descriptors *did* exhibit issues with spurious peaks so improving this is a priority (using a range parameter) [now improved]
* Current median filter is weird for low spans? (needs fixing but can't repro)

*  -- Stats --  [all corrected - check on the length/position decisions!!! - possible further testing]

* trough and trough_pos *did* return infs due to an incorrect test [now corrected]
* ratio_below *did* return inf due to an incorrectly formed test [now corrected]
* crossing_trough / crossing_trough_pos / cross_below / crossings_below *did* search incorrectly (giving values above thresh) [now corrected]
* longest_crossings_above and longest_crossings_below *did* return allow the end to be beyond length (due to masktime) [now corrected]
* longest_cross_above and longest_cross_below *did* return the lengths of the crossings (not as documented) [now corrected]
* all peak searches *did* incorrectly detect peaks for each stage of continued upward motion (rather than a single peak) [now corrected]
* peak and trough searches *did not* address infinity values [now corrected]

-- TO DO --

* Need to check volume compensation points (and which descriptors are level variant or not) - leave as is
  
  - variant - rms / abs / peakamp / energy / loudness / spectral peaks (levels only)
  - invariant - energy_ratio / rolloff / spectral_crest / sfm / lin + log shape / pitch / confidence / lin + log brightness / inharmonicity / roughness / noise_ratio / harmonic_ratio / foote
  - variable - flux / mkl
  
* Need to investigate zero inputs and min return values (applies only to dB currently set to - 180dB suggest leave for now)
  - 0 - peak freqs / peak amps (both types) / confidence / foote / flux / mkl
  - -180.0dB (or 0 if not in dB) - rms / abs / peakamp / energy / loudness
  - inf - energy_ratio / rolloff / spectral_crest / sfm / lin + log shape / pitch  / lin + log brightness / inharmonicity / roughness / noise_ratio / harmonic_ratio

  Changes are:
  1 - foote and mkl to 0.0 for difference between silent frames
  2 - pitch to inf for no pitch found
  
* Need to check lags and other things with fftparams that have mismatch window and FFT [all correct]

* Look at user messages/feedback [completed] 
  
* Need to investigate padding issues

* Need to investigate speeds

-- DECISIONS --

* Stats - return crossing position (as documented) or length for relevant searches [yes - corrected]
* Descriptors - implement power option for some descriptors? [all shape related? - rolloff / crest / sfm / lin and log shape / brightness ]??? [not now - nothing to do]
* sfm convert to db (correct conversions for powers) [added for amplitude - is now the default]
* Content descriptors - implement better filtering for partials (including interface for inharmonicity which has an additional parameter) [corrected - inharmonicity changed]
* spectral_peaks - unify returns and interface (invalid freqs/amps and db conversion on/off) [corrected - RT now defaults to db]

* How to deal with padding


-- Desciptors Notes --

Fully Working:
[7] - abs / peakamp / rms / energy / loudness / energy_ratio / lin_centroid
Minor Improvements:
[8] - rolloff / foote / log_centroid / log_spread / pitch / confidence / lin_brightness / log_brightness
Bug fixes [no questions]:
[9] - noise_ratio / harmonic_ratio / lin_spread / flux / mkl / lin_skewness / lin_kurtosis / log_skewness / log_kurtosis

[24]

Bug fixes [questions]:
[3] contents - spectral_peaks / inharmonicity / roughness - spurious peaks
[2] sfm / spectral_crest (should there be a power option and how to convert the final ratio? - should sfm report in db?)
