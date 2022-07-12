AHarker Externals
=================

A Set of 80+ Externals for a variety of tasks in Cycling 74's Max.

By Alex Harker

**Contact:**

* ajharker@gmail.com 
* http://www.alexanderjharker.co.uk/
			
A brief overview of some areas addressed:

* general purpose scaling for Max and MSP
* efficient partitioned + non- partitioned convolution
* comprehensive descriptor analysis (realtime + non-realtime)
* enhanced audio multi-threading / dynamic patch loading
* efficient buffer playback and storage
* high quality random number generators for Max and MSP
* sample accurate voice management and more
* thread debugging and switching
* utility objects
* SIMD versions of 35 basic MSP objects

The AHarker Externals are licensed under a the 3-clause BSD license.

Portions of this work have been supported by the Arts and Humanities Research Council, and the HISSTools Project at the University of Huddersfield.

Compiling
---------

Should you wish to compile you will require the following:

**Mac:**
- XCode
- The max-sdk-base repository (https://github.com/Cycling74/max-sdk-base)

The max-sdk-base repository should be placed parallel to (in the same encolsing folder) as this repository so that b
The location for the max-sdk-base can be modified in Config_AHarker_Externals.xcconfig if desired.
AVX / SSE support can also be toggled in Config_AHarker_Externals.xcconfig (for old machines, or Rosetta 2 support).

**Windows:**
- Visual Studio
- OneMKL or OneAPI Base Toolkit (which includes OneMKL)
    - OneMKL https://www.intel.com/content/www/us/en/developer/tools/oneapi/onemkl-download.html
    - OneAPI Base Toolkit https://www.intel.com/content/www/us/en/developer/tools/oneapi/base-toolkit-download.html
- The max-sdk-base repository (https://github.com/Cycling74/max-sdk-base)

The OneMKL installation needs to include Visual Studio integration (this is required for some of the externals using SIMD)
The max-sdk-base repository should be placed parallel to (in the same encolsing folder) as this repository so that b
The location for the max-sdk-base can be modified in AH_Win_Debug.props / AH_Win_Release.props if desired.

Enjoy,

Alex Harker

