
/*
 *  macaddress
 *
 *  macaddress is an object for obtaining the MAC address of a computer's wireless hardware, for use as a unique identifier.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */

#ifdef __APPLE__
#include <SystemConfiguration/SystemConfiguration.h>
#include <CoreFoundation/CoreFoundation.h>
#else
#include <winsock2.h>
#include <iphlpapi.h>
#include <cstring>
#include <vector>
#endif

#include <ext.h>
#include <ext_obex.h>

#include <string>

t_class *this_class;

enum class match_type
{
    any,
    wifi,
    ethernet,
    name
};

struct t_macaddress
{
    t_object a_obj;
    
    match_type mode;
    t_symbol *name;
    
    void *output;
};


t_symbol *ps_empty;
t_symbol *ps_failed;
t_symbol *ps_any;
t_symbol *ps_wifi;
t_symbol *ps_ethernet;


void *macaddress_new(t_symbol *sym);
void macaddress_bang(t_macaddress *x);
void macaddress_set(t_macaddress *x, t_symbol *sym);

void macaddress_set_internal(t_macaddress *x, t_symbol *sym);

void macaddress_assist(t_macaddress *x, void *b, long m, long a, char *s);


int C74_EXPORT main()
{
    this_class = class_new ("macaddress",
                            (method)macaddress_new,
                            nullptr,
                            (short) sizeof(t_macaddress),
                            nullptr,
                            A_DEFSYM,
                            0);
    
    class_addmethod (this_class, (method)macaddress_bang, "bang", 0);
    class_addmethod (this_class, (method)macaddress_set, "set", A_SYM, 0);
    class_addmethod (this_class, (method)macaddress_assist, "assist", A_CANT, 0);
    
    class_register (CLASS_BOX, this_class);
    
    ps_empty = gensym("");
    ps_failed = gensym("failed");
    ps_failed = gensym("any");
    ps_wifi = gensym("wifi");
    ps_ethernet = gensym("ethernet");

    return 0;
}

void *macaddress_new(t_symbol *sym)
{
    t_macaddress *x = (t_macaddress *) object_alloc(this_class);
    
    x->output = outlet_new(x, 0);
    macaddress_set_internal(x, sym);
    
    return x;
}

void macaddress_set(t_macaddress *x, t_symbol *sym)
{
    macaddress_set_internal(x, sym);
    macaddress_bang(x);
}

void macaddress_set_internal(t_macaddress *x, t_symbol *sym)
{
    auto set_type = [&](match_type mode)
    {
        x->mode = mode;
        x->name = nullptr;
    };
    
    // Replace empty string
    
    if (sym == ps_empty)
        sym = ps_wifi;
    
    // Set mode and object variables
    
    if (sym == ps_any)
        set_type(match_type::any);
    else if (sym == ps_wifi)
        set_type(match_type::wifi);
    else if (sym == ps_ethernet)
        set_type(match_type::ethernet);
    else
    {
        x->mode = match_type::name;
        x->name = sym;
    }
}

#ifdef __APPLE__
std::string get_string(CFStringRef str)
{
    char cString[(CFStringGetLength(str) * 4) + 1];
    CFStringGetCString(str, cString, sizeof(cString), kCFStringEncodingUTF8);
    
    return cString;
}

void macaddress_bang(t_macaddress *x)
{
    CFStringRef mac_address = nullptr;
    SCNetworkInterfaceRef found_interface = nullptr;
        
    CFArrayRef interface_array = SCNetworkInterfaceCopyAll();
    unsigned long num_interfaces = CFArrayGetCount(interface_array);
    
    for (unsigned long i = 0; i < num_interfaces; i++)
    {
        SCNetworkInterfaceRef interface = (SCNetworkInterfaceRef) CFArrayGetValueAtIndex(interface_array, i);
        
        CFStringRef str1 = nullptr;
        CFStringRef str2 = nullptr;

        auto match = [&](CFStringRef comparison)
        {
            if (!CFStringCompare(str1, comparison, 0))
                found_interface = interface;
                
            return found_interface;
        };
        
        if (x->mode == match_type::name)
        {
            // Compare to BSD name and then display name
            
            str1 = CFStringCreateWithCString(kCFAllocatorDefault, x->name->s_name, kCFStringEncodingUTF8);
            str2 = SCNetworkInterfaceGetBSDName(interface);

            if (!match(str2))
                match(SCNetworkInterfaceGetLocalizedDisplayName(interface));
            
            CFRelease(str1);
        }
        else
        {
            // Compare type
            
            str1 = SCNetworkInterfaceGetInterfaceType(interface);

            switch (x->mode)
            {
                case match_type::wifi:          match(kSCNetworkInterfaceTypeIEEE80211);    break;
                case match_type::ethernet:      match(kSCNetworkInterfaceTypeEthernet);     break;
                default:                        found_interface = interface;
            }
        }
        
        if (found_interface)
            break;
    }
    
    if (found_interface)
        mac_address = SCNetworkInterfaceGetHardwareAddressString(found_interface);
    
    if (mac_address)
        outlet_anything(x->output, gensym(get_string(mac_address).c_str()), 0, nullptr);
    else
        outlet_anything(x->output, ps_failed, 0, nullptr);
    
    // We need to release anything that is created via copy...
    
    CFRelease(interface_array);
}
#else
std::string UTF16ToUTF8(const std::wstring& wstr)
{
    int requiredSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, nullptr, 0, nullptr, nullptr);

    if (requiredSize > 0)
    {
        std::vector<char> buffer(requiredSize);
        if (WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &buffer[0], requiredSize, nullptr, nullptr))
            return buffer.data();
    }

    return "";
}

void macaddress_bang(t_macaddress *x)
{
    char mac_address[32];
    char temp_string[32];
    DWORD ret_val = 0;
    
    IP_ADAPTER_ADDRESSES *addresses = nullptr;
    IP_ADAPTER_ADDRESSES *current_address = nullptr;
    IP_ADAPTER_ADDRESSES *found_address = nullptr;
    ULONG out_buf_length = 15000;
    
    mac_address[0] = 0;
    
    // If out_buf_length is insufficient then the second call should function correctly

    for (int i = 0; i < 2; i++)
    {
        addresses = (IP_ADAPTER_ADDRESSES *) malloc(out_buf_length);
        if (addresses == nullptr)
            break;
        
        ret_val = GetAdaptersAddresses(AF_INET, GAA_FLAG_INCLUDE_PREFIX, nullptr, addresses, &out_buf_length);
        
        if (ret_val == NO_ERROR)
            break;
        
        free(addresses);
        addresses = nullptr;
    }
    
    if (ret_val == NO_ERROR)
    {        
        for (current_address = addresses; current_address; current_address = current_address->Next)
        {
            if (current_address->PhysicalAddressLength)
            {
                bool found = false;

                switch (x->mode)
                {
                    case match_type::any:
                        found = true;
                        break;
                        
                    case match_type::wifi:
                        found = current_address->IfType == IF_TYPE_IEEE80211;
                        break;
                        
                    case match_type::ethernet:
                        found = current_address->IfType == IF_TYPE_ETHERNET_CSMACD ||
                                current_address->IfType == IF_TYPE_ETHERNET_3MBIT ||
                                current_address->IfType == IF_TYPE_FASTETHER ||
                                current_address->IfType == IF_TYPE_FASTETHER_FX ||
                                current_address->IfType == IF_TYPE_GIGABITETHERNET;
                        break;
                        
                    default:
                        if (!strcmp(x->name->s_name, UTF16ToUTF8(current_address->FriendlyName).c_str()) ||
                            !strcmp(x->name->s_name, current_address->AdapterName))
                            found = true;
                }

                if (found)
                {
                    found_address = current_address;
                    break;
                }
            }
        }
    }
    
    if (found_address)
    {
        for (unsigned long i = 0; i < current_address->PhysicalAddressLength; i++)
        {
            if (i == (current_address->PhysicalAddressLength - 1))
                sprintf(temp_string, "%.2x", (int)current_address->PhysicalAddress[i]);
            else
                sprintf(temp_string, "%.2x:", (int)current_address->PhysicalAddress[i]);
            strcat(mac_address, temp_string);
        }
    }

    if (addresses)
        free(addresses);
    
    if (found_address)
        outlet_anything(x->output, gensym(mac_address), 0, nullptr);
    else
        outlet_anything(x->output, ps_failed, 0, nullptr);
}
#endif

void macaddress_assist(t_macaddress *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"Bang to Output");
    else
        sprintf(s,"MAC Address");
}
