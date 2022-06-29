
/*
 *  recursivefolder
 *
 *  recursivefolder is a version of folder that allows recursive searching of a folder.
 *  Searching may be either infinitly recursive or to a specified depth.
 *
 *  Copyright 2010-22 Alex Harker. All rights reserved.
 *
 */


#include <ext.h>
#include <ext_obex.h>

#include <algorithm>


// Globals and Object Structure

t_class *this_class;

t_symbol *ps_folder = gensym("folder");
t_symbol *ps_start = gensym("start");
t_symbol *ps_end = gensym("end");
t_symbol *ps_empty = gensym("empty");

constexpr long max_types = 128;

struct t_recursive_folder
{
    t_object f_ob;
    
    t_filepath f_path;
    t_symbol *f_input;
    
    t_fourcc f_types[max_types];
    
    t_atom_long f_recursion;
    long f_numtypes;

    long f_count;

    void *f_info_out;
    void *f_count_out;
};

// Function Prototypes

void *recursive_folder_new(t_symbol *s, long argc, t_atom *argv);
void recursive_folder_assist(t_recursive_folder *x, void *b, long m, long a, char *s);

void recursive_folder_bang(t_recursive_folder *x);
void recursive_folder_anything(t_recursive_folder *x, t_symbol *s, long argc, t_atom *argv);
void recursive_folder_action(t_recursive_folder *x);
void recursive_folder_lookup(t_recursive_folder *x);
short recursive_folder_enumerate(t_recursive_folder *x, t_filepath f_path, t_symbol *f_name, t_atom_long recursion);

void recursive_folder_types(t_recursive_folder *x, t_symbol *s, long argc, t_atom *argv);

// Main

int C74_EXPORT main()
{
    t_class *c;
    
    c = class_new("recursivefolder",
                  (method) recursive_folder_new,
                  (method) nullptr,
                  sizeof(t_recursive_folder),
                  (method) nullptr,
                  A_GIMME,
                  0);
    
    class_addmethod(c, (method) recursive_folder_bang, "int", A_LONG, 0);
    class_addmethod(c, (method) recursive_folder_bang, "bang", 0);
    class_addmethod(c, (method) recursive_folder_anything, "anything", A_GIMME, 0);
    class_addmethod(c, (method) recursive_folder_anything, "symbol", A_SYM, 0);
    class_addmethod(c, (method) recursive_folder_types, "types", A_GIMME, 0);
    class_addmethod(c, (method) recursive_folder_assist, "assist", A_CANT, 0);
    
    class_register(CLASS_BOX, c);
    this_class = c;
    
    return 0;
}

// New / Assist

void *recursive_folder_new(t_symbol *s, long argc, t_atom *argv)
{
    t_recursive_folder *x = (t_recursive_folder *) object_alloc(this_class);
    
    x->f_count_out = intout((t_object *) x);
    x->f_info_out = outlet_new((t_object *) x, nullptr);
    
    x->f_input = 0;
    x->f_numtypes = 0;
    x->f_path = 0;
    x->f_count = 0;
    x->f_recursion = 0;
    
    if (argc)
    {
        if (atom_gettype(argv + argc - 1) == A_LONG)
        {
            x->f_recursion = atom_getlong(argv + argc - 1);
            argc--;
        }
        recursive_folder_types(x, s , argc, argv);
    }
    
    return x;
}

void recursive_folder_assist(t_recursive_folder *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
    {
        switch (a)
        {
            case 0: sprintf(s,"Path Name of Folder to List"); break;
        }
    }
    else
    {
        if (m == ASSIST_OUTLET)
        {
            switch (a)
            {
                case 0: sprintf(s,"File Name List"); break;
                case 1: sprintf(s,"Count of Items Listed"); break;
            }
        }
    }
}

// Search (bang)

void recursive_folder_bang(t_recursive_folder *x)
{
    defer(x, (method) recursive_folder_action, 0, 0, 0);
}

// Set Folder Name + Search

void recursive_folder_anything(t_recursive_folder *x, t_symbol *s, long argc, t_atom *argv)
{
    x->f_input = s;
    x->f_path = 0;
    recursive_folder_bang(x);
}

// Do Recursive Folder Search

void recursive_folder_action(t_recursive_folder *x)
{
    char sourcename[2048], fullname[2048];
    t_filepath path;
    short err;
    char char_null = 0;
    x->f_count = 0;
    
    if (x->f_path == 0)
    {
        if (x->f_input)
        {
            strcpy(sourcename, x->f_input->s_name);
            err = path_frompathname(sourcename, &path, fullname);
            if (err || fullname[0])
            {
                object_error((t_object *) x, "%s: not a folder", x->f_input->s_name);
                x->f_path = 0;
            }
            else
                x->f_path = path;
        }
    }
    
    if (x->f_path)
    {
        strcpy(sourcename, x->f_input->s_name);
        if (!path_topathname(x->f_path, &char_null, fullname))
            recursive_folder_enumerate(x, x->f_path, gensym(fullname), 0);
    }
    
    outlet_int(x->f_count_out, x->f_count);
}

// Enumerate Items Recursively

short recursive_folder_enumerate(t_recursive_folder *x, t_filepath f_path, t_symbol *f_name, t_atom_long recursion)
{
    t_filepath new_path = 0;
    t_fileinfo info;
    t_fourcc type;
    
    t_atom out_atom[3];
    
    bool match;
    
    char fullname[2048];
    char tempname[2048];
    char name[512];
    
    short started = 0;
    
    if (!f_path)
        return 0;
    
    void *fold = path_openfolder(f_path);
    
    if (!fold)
        return 0;
    
    atom_setsym(out_atom, ps_start);
    atom_setsym(out_atom + 1, f_name);
    
    while (path_foldernextfile(fold, &type, name, false))
    {
        if (x->f_numtypes == 0)
            match = true;
        else
        {
            match = false;
            
            if (!type)
            {
                // N.B. Do not pass typelist in here....
                
                if (!path_extendedfileinfo(name, f_path, &info, 0, 0, true))
                    type = info.type;
                else
                    type = 0;
            }
            for (long i = 0; i < x->f_numtypes; i++)
            {
                if (type == x->f_types[i])
                {
                    match = true;
                    break;
                }
            }
        }
        
        if (match)
        {
            if (!started)
                outlet_anything(x->f_info_out, ps_folder, 2L, out_atom);
            
            path_topathname(f_path, name, fullname);
            
            outlet_anything(x->f_info_out, gensym(fullname), 0 , nullptr);
            x->f_count++;
            started |= 2;
        }
        
        if (type == 'fold' && (!x->f_recursion || x->f_recursion - 1 > recursion))
        {
            if (!started)
                outlet_anything(x->f_info_out, ps_folder, 2, out_atom);
            
            path_topathname(f_path, name, fullname);
            if (!path_frompathname(fullname, &new_path, tempname))
                started |= recursive_folder_enumerate(x, new_path, gensym(fullname), recursion + 1);
        }
    }
    path_closefolder(fold);
    
    if (!started)
    {
        atom_setsym(out_atom, ps_empty);
        outlet_anything(x->f_info_out, ps_folder, 2, out_atom);
    }
    else
    {
        atom_setsym(out_atom, ps_end);
        atom_setlong(out_atom + 2, started / 2);
        outlet_anything(x->f_info_out, ps_folder, 3, out_atom);
    }
    
    if (!started)
        started = 1;
    
    return started;
}

// Set Search Types

void recursive_folder_types(t_recursive_folder *x, t_symbol *s, long argc, t_atom *argv)
{
    uint8_t type[4];
    
    x->f_numtypes = 0;
    
    argc = std::min(argc, max_types);
    
    while (argc--)
    {
        if (atom_gettype(argv) == A_SYM)
        {
            size_t len = strlen(atom_getsym(argv)->s_name);
            for (long i = 0; i < 4; i++)
                type[i] = (i >= len) ? ' ' : atom_getsym(argv)->s_name[i];
            STR_TO_FOURCC(*((t_fourcc *)type));
            sysmem_copyptr(type, &x->f_types[x->f_numtypes++], 4L);
        }
        argv++;
    }
}
