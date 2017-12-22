
/*
 *  macaddress
 *
 *	macaddress is an object for obtaining the MAC address of a computer's wireless hardware, for use as a unique identifier.
 *
 *  Copyright 2010 Alex Harker. All rights reserved.
 *
 */

#ifdef __APPLE__
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#else
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#endif

#include <ext.h>
#include <ext_obex.h>


t_class *this_class;


typedef struct macaddress{
	
	t_object a_obj;
	
    void *output;
	
} t_macaddress;


t_symbol *ps_failed;


void *macaddress_new();
void macaddress_free(t_macaddress *x);
void macaddress_bang(t_macaddress *x);

void macaddress_assist(t_macaddress *x, void *b, long m, long a, char *s);


int C74_EXPORT main(void)
{
	this_class = class_new ("macaddress",
							(method)macaddress_new,
							(method)macaddress_free,
							(short) sizeof(t_macaddress), 
							NULL,
							0);	
	
	class_addmethod (this_class, (method)macaddress_bang, "bang", 0);
	class_addmethod (this_class, (method)macaddress_assist, "assist", A_CANT, 0);
	
	class_register (CLASS_BOX, this_class);
	
	ps_failed = gensym("failed");
	
	return 0;
}

void *macaddress_new()
{
    t_macaddress *x = (t_macaddress *) object_alloc(this_class);
	
	x->output = outlet_new(x, 0);
	
    return x;
	
}

void macaddress_free(t_macaddress *x)
{
}

#ifdef __APPLE__

void macaddress_bang(t_macaddress *x)
{
	CFStringRef mac_address = NULL;
    
    CFStringRef interface_type;
	CFArrayRef interface_array;
	SCNetworkInterfaceRef next_interface, found_interface;
	
	char const *mac_address_str;
	unsigned long found = 0;
	unsigned long i, loop;
	
	interface_array = SCNetworkInterfaceCopyAll();
	loop = CFArrayGetCount(interface_array);
	
	for (i = 0; i < loop; i++)
	{
		next_interface = CFArrayGetValueAtIndex(interface_array, i);
		interface_type = SCNetworkInterfaceGetInterfaceType(next_interface);
        /*
        CFStringRef interface_bsd_name = SCNetworkInterfaceGetBSDName(next_interface);
        
        if (interface_bsd_name != NULL)
            object_post((t_object *)x, "BSD Name %s", CFStringGetCStringPtr(interface_bsd_name, CFStringGetFastestEncoding(interface_bsd_name)));
        else
            object_post((t_object *)x, "NULL BSD Name");
        
        if (interface_type != NULL)
            object_post((t_object *)x, "Interface Type %s", CFStringGetCStringPtr(interface_type, CFStringGetFastestEncoding(interface_type)));
        else
            object_post((t_object *)x, "NULL Interface type");
        */
        
		if(!CFStringCompare(interface_type, kSCNetworkInterfaceTypeIEEE80211, 0))
		{
			found = 1; 
			found_interface = next_interface;
			break;
		}
	}
	
	if (found)
		mac_address = SCNetworkInterfaceGetHardwareAddressString(found_interface);
	
	if (mac_address)
	{
		mac_address_str = CFStringGetCStringPtr(mac_address, CFStringGetFastestEncoding(mac_address));
		outlet_anything(x->output, gensym((char *)mac_address_str), 0, NIL);
	}
	else
		outlet_anything(x->output, ps_failed, 0, NIL);
    
    // We need to release anything that is created via copy...
    
    CFRelease(interface_array);
}

#else

void macaddress_bang(t_macaddress *x)
{			
	char mac_address[32];
	char temp_string[32];
	DWORD ret_val = 0;
	unsigned int found = 0;
	unsigned int i = 0;
		
	IP_ADAPTER_ADDRESSES addresses = NULL;
	IP_ADAPTER_ADDRESSES current_address = NULL;
	ULONG outBufLen = 15000;
	
	mac_address[0] = 0;

	for (i = 0; i < 2; i++)
	{
		addresses = (IP_ADAPTER_ADDRESSES *) malloc(outBufLen);
		if (addresses == NULL)
			break;
		
		ret_val = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, NULL, addresses, &outBufLen);
		
		if (ret_val == NO_ERROR)
			break;

		free(addresses);
		addresses = NULL; 
	} 
		
	if (ret_val == NO_ERROR) 
	{
		current_address = addresses;

		while (current_address)
		{
			if (current_address->PhysicalAddressLength && current_address->IfType == IF_TYPE_IEEE80211)
			{
				for (i = 0; i < current_address->PhysicalAddressLength; i++) 
				{
                    if (i == (current_address->PhysicalAddressLength - 1))
                        sprintf(temp_string, "%.2x", (int) current_address->PhysicalAddress[i]);
                    else
                        sprintf(temp_string, "%.2x:",(int) current_address->PhysicalAddress[i]);
					strcat(mac_address, temp_string);
                }
				found = 1;
				break;
			}
			current_address = current_address->Next;
		}
	}
		
	if (addresses)
		free(addresses);
		
	if (found)
		outlet_anything(x->output, gensym((char *)mac_address), 0, NIL);
	else
		outlet_anything(x->output, ps_failed, 0, NIL);
}

#endif

void macaddress_assist(t_macaddress *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET) 
		sprintf(s,"Bang to Output");
	else
		sprintf(s,"MAC Address");	
}
