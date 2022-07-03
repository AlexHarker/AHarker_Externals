** Issues **

General and Small

* Padding issue (not yet found)
* Old no RT object didn't respond to different buffer sample rates correctly (need to reset fft params)
* Precision etc. - some small differences in various places
* Shape desciptors (crest/sfm/skewness/kurtosis) - some differences for large fft with sine input

* Descriptors

* Rolloff *now* has interpolation (so reports slightly lower) and uses power (as Peeters / unlike flucoma default)
* SFM *was* using power rather than amplitudes
* Spectral crest *was* using power rather than amplitudes
* Linear spread *was* squaring the raw value before the Hz adjustment (so not a useful value)
* Higher spectral shapes (skewness + kurtosis) *were* wrong in early versions (lack of division by the overall amplitude sum)
* Log higher spectral shapes *Were* still wrong in more recent versions
* Pitch modules *had* a significant issue where pitch reports as 0 when invalid / there is a precision effect
* Pitch *now* reports as inf for no pitch (not zero) solving averaging and stats errors
* Flux *had* a fixed index bug in the code producing consistently incorrect results
* Foote *did* return inf for the change between two zero frames (now returns zero which seems more correct)
* Noise ratio *did* have errors in the median filter and the bin indexing, meaning fairly meaningless results (now fixed)
* Roughness *did not* correctly include the hack for min amp (original code no longer available but may be on old hard disk)
* RT spectral_peaks reports in linear amps but non RT in db (with no options)
* Summary spectral peak finding returns zeros if it can't find enough peaks, not infs
* Median filtering implementation *was* broken
* All content descriptors *did* exhibit issues with spurious peaks so improving this is a priority (using a range parameter)
* Current median filter is weird for low spans? (needs fixing but can't repro)

* Stats

* trough and trough_pos *did* return infs due to an incorrect test
* crossing_trough / crossing_trough_pos / cross_below / crossings_below *did* search incorrectly (giving values above thresh)
* longest_crossings_above and longest_crossings_below *did* return allow the end to be beyond length (due to masktime)
* longest_cross_above and longest_cross_below *did* return the lengths of the crossings (not as documented)
* all peak searches *did* incorrectly detect peaks for each stage of continued upward motion (rather than a single peak)
* peak and trough searches *did not* address infinity values

TO DO

* Need to check volume compensation points (and which descriptors are level variant or not
* Need to check lags and other things with fftparams that have mismatch window and FFT
* Need to investigate speeds
* Need to investigate zero inputs
* Need to investigate min return values