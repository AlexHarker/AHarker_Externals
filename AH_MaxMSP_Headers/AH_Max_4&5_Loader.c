
/*
 *  AH_Max_4&5_Loader
 *
 *	This file is used in objects that compile seperately for Max 4 and 5 to load the correct bundle at loadtime.
 *
 *	Both bundles should be included in the "Plug-ins" folder of the enclosing bundle (this can be done using a script in XCode).
 *	The names of the enclosing bundle, as well as the Max 4 and 5 versions should be defined in a header file named "External_Bundle_Identifiers.h".
 *	The name stings should be defined as BUNDLE_ID, MAX_4_EXEC, and MAX_5_EXEC
 *	This file is then compiled as the only source file needed for the enclosing bundle.
 *
 *	When MaxMSP loads the external this code detemines the version of MaxMSP running and then will load the correct bundle,
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <External_Bundle_Identifiers.h>

typedef long (*func_ptr) ();

int main ()
{
	CFBundleRef max_version_bundle = NULL;
	CFStringRef bundle_identifier;
	CFBundleRef this_bundle;
	CFURLRef this_URL;
	CFArrayRef the_bundles;
	CFIndex i;

	// Find out which bundle we are looking for....

	if ((maxversion() & 0x3FFF) >= 0x500)
		bundle_identifier = CFSTR(MAX_5_EXEC);
	else
		bundle_identifier = CFSTR(MAX_4_EXEC);
		
	// Find this bundle (the loading bundle)

	this_bundle = CFBundleGetBundleWithIdentifier(CFSTR(BUNDLE_ID));
	
	// Now get its "Plug-ins" folder URL and all internal bundles
	
	this_URL = CFBundleCopyBuiltInPlugInsURL (this_bundle);
	the_bundles = CFBundleCreateBundlesFromDirectory (kCFAllocatorDefault , this_URL, NULL);


	// Find the relevant bundle within this bundle

	for (i = 0; i < CFArrayGetCount(the_bundles); i++)
	{
		CFBundleRef test_bundle = (CFBundleRef) CFArrayGetValueAtIndex (the_bundles, i);
		CFStringRef test_identifier = CFBundleGetIdentifier (test_bundle);

		if (test_identifier && CFStringCompare (test_identifier, bundle_identifier, 0) == kCFCompareEqualTo && !max_version_bundle)
		{
			max_version_bundle = test_bundle;
			CFRetain(max_version_bundle);
		}
		else
		{
			if (test_bundle)
				CFRelease(test_bundle);
		}
	}	

	// Now load the relevant bundle

	if (max_version_bundle)
	{
		func_ptr main_ptr = CFBundleGetFunctionPointerForName (max_version_bundle, CFSTR("main"));
		main_ptr();
		
	}

	// Release whatever we should release
	
	CFRelease(the_bundles);
	CFRelease(this_URL);
	
	return 0;
}