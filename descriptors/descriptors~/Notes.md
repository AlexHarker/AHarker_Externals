** Issues **

General and Small

* Padding issue (not yet found)
* Old no RT object didn't respond to different buffer sample rates correctly (need to reset fft params) [ now corrected ]
* Precision etc. - some small differences in various places [ note only ]
* Shape desciptors (crest/sfm/skewness/kurtosis) - some differences for large fft with sine input [note only]

* Descriptors

* Rolloff *now* has interpolation (so reports slightly lower) - N.B. it uses power (as Peeters / unlike flucoma default)
* SFM *was* using power rather than amplitudes [now corrected]
* Spectral crest *was* using power rather than amplitudes [now corrected]
* Linear spread *was* squaring the raw value before the Hz adjustment (so not a useful value) [now corrected]
* Higher spectral shapes (skewness + kurtosis) *were* wrong in early versions (lack of division by the overall amplitude sum) [now corrected]
* Log higher spectral shapes *Were* still wrong in more recent versions [now corrected]
* Pitch modules *had* a significant issue where pitch reports as 0 when invalid / there is a precision effect [now corrected]
* Pitch *now* reports as inf for no pitch (not zero) solving averaging and stats errors 
* Flux *had* a fixed index bug in the code producing consistently incorrect results [now corrected]
* Foote *did* return inf for the change between two zero frames (now returns zero which seems more correct) [now corrected]
* Noise ratio *did* have errors in the median filter and the bin indexing, meaning fairly meaningless results [now corrected]
* Roughness *did not* correctly include the hack for min amp (original code no longer available but may be on old hard disk)
* RT spectral_peaks reports in linear amps but non RT in db (with no options)
* Summary spectral peak finding returns zeros if it can't find enough peaks, not infs
* Median filtering implementation *was* broken [now corrected]
* All content descriptors *did* exhibit issues with spurious peaks so improving this is a priority (using a range parameter) [now improved]
* Current median filter is weird for low spans? (needs fixing but can't repro)

* Stats

* trough and trough_pos *did* return infs due to an incorrect test [now corrected]
* crossing_trough / crossing_trough_pos / cross_below / crossings_below *did* search incorrectly (giving values above thresh) [now corrected]
* longest_crossings_above and longest_crossings_below *did* return allow the end to be beyond length (due to masktime) [now corrected]
* longest_cross_above and longest_cross_below *did* return the lengths of the crossings (not as documented) [now corrected]
* all peak searches *did* incorrectly detect peaks for each stage of continued upward motion (rather than a single peak) [now corrected]
* peak and trough searches *did not* address infinity values [now corrected]

TO DO

* Need to check volume compensation points (and which descriptors are level variant or not
  
  - variant - rms / abs / peakamp / energy / loudness / spectral peaks (levels only)
  - invariant - energy_ratio / rolloff / spectral_crest / sfm / lin + log shape / pitch / confidence / lin + log brightness / inharmonicity / roughness / noise_ratio / harmonic_ratio / foote
  - variable - flux / mkl
  
* Need to investigate zero inputs and min return values (applies only to dB currently set to - 180dB suggest leave for now)
  - 0 - peak freqs / peak amps (both types) / confidence / foote / flux
  - -180.0dB (or 0 if not in dB) - rms / abs / peakamp / energy / loudness
  - inf - energy_ratio / rolloff / spectral_crest / sfm / lin + log shape / pitch  / lin + log brightness / inharmonicity / roughness / noise_ratio / harmonic_ratio

  Change are:
  1 - foote to 0.0 for difference between silent frames
  2 - pitch to inf for no pitch found
  
* Need to check lags and other things with fftparams that have mismatch window and FFT

* Need to investigate padding issues

* Need to investigate speeds
