
#include "patch_slot.hpp"

#include <algorithm>
#include <jpatcher_api.h>


#ifndef __APPLE__
#define snprintf _snprintf
#endif

struct symbol_binding
{
    symbol_binding(const char *str, void *item) : m_symbol(gensym(str)), m_previous(m_symbol->s_thing)
    {
        m_symbol->s_thing = reinterpret_cast<t_object *>(item);
    }

    ~symbol_binding()
    {
        m_symbol->s_thing = m_previous;
    }

    t_symbol *m_symbol;
    t_object *m_previous;
};

// Deferred patch deletion

void delete_patch(t_object *patch, t_symbol *s, short argc, t_atom *argv)
{
    object_free(patch);
}

// Set subpatcher association

int set_subpatcher_assoc(t_patcher *p, t_object *x)
{
    t_object *assoc;

    object_method(p, gensym("getassoc"), &assoc);
    if (!assoc)
        object_method(p, gensym("setassoc"), x);

    return 0;
}

// Patcher traversal

template <void (patch_slot::*Method)(t_patcher *p)>
int patch_slot::patcher_traverse(t_patcher *p, void *arg, t_object *owner)
{
    // Avoid recursion into a poly / pfft / dynamicdsp~

    t_object *assoc = 0;
    object_method(p, gensym("getassoc"), &assoc);
    if (assoc && assoc != owner)
        return 0;

    // Call function

    (this->*Method)(p);

    // Continue search for subpatchers and traverse them if found

    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        t_patcher *p2;
        long index = 0;

        while (b && (p2 = reinterpret_cast<t_patcher *>(object_subpatcher(jbox_get_object(b), &index, arg))))
            if (patcher_traverse<Method>(p2, arg, owner))
                return 1;
    }

    return 0;
}

patch_slot::~patch_slot()
{
    free_patch();
}

// Loading

patch_slot::load_error patch_slot::load(t_symbol *path, long argc, t_atom *argv, long vec_size, long sampling_rate)
{
    // Copy Arguments

    m_path_symbol = path;
    m_name = std::string(path->s_name);
    m_path = 0;
    m_argc = std::min(argc, max_args());

    std::copy_n(argv, m_argc, m_argv);

    // Load

    return load(vec_size, sampling_rate, true);
}

patch_slot::load_error patch_slot::load(long vec_size, long sampling_rate, bool initialise)
{
    t_fourcc type;
    t_fourcc valid_types[3];

    // Set the valid types to test for

    valid_types[0] = FOUR_CHAR_CODE('maxb');
    valid_types[1] = FOUR_CHAR_CODE('TEXT');
    valid_types[2] = FOUR_CHAR_CODE('JSON');

    // Set flags off / free old patch / set on ready to be changed at load time

    bool on = initialise ? true : m_on;
    m_valid = m_busy = m_on = false;
    free_patch();
    m_on = on;

    // Try to locate a file of the given name that is of the correct type

    char name[512];
    strcpy(name, m_name.c_str());

    if (locatefile_extended(name, &m_path, &type, valid_types, 3))
        return load_error::file_not_found;

    // Store the binding symbols with RAII to restore once done

    symbol_binding owner("__dynamic.host_object__", m_owner);
    symbol_binding vis("inhibit_subpatcher_vis", reinterpret_cast<void *>(-1));
    symbol_binding PAT("#P", m_parent);

    // Load the patch (don't interrupt dsp or send loadbang yet)
    
    short saved_loadupdate = dsp_setloadupdate(false);
    loadbang_suspend();
    m_patch = reinterpret_cast<t_patcher *>(intload(name, m_path, 0 , static_cast<short>(m_argc), m_argv, false));

    // Check something has loaded and that it is a patcher

    if (!m_patch)
        return load_finished(load_error::nothing_loaded, saved_loadupdate);

    if (!ispatcher(reinterpret_cast<t_object *>(m_patch)))
        return load_finished(load_error::not_patcher, saved_loadupdate);

    // Use setclass to allow Modify Read-Only / Set associations

    long result = 0;
    object_method(m_patch, gensym("setclass"));
    object_method(m_patch, gensym("traverse"), set_subpatcher_assoc, m_owner, &result);

    // Find ins and link outs

    patcher_traverse<&patch_slot::find_ins>(m_patch, nullptr, m_owner);
    patcher_traverse<&patch_slot::link_outlets>(m_patch, nullptr, m_owner);

    // Set window name and compile DSP if the owning patch is on

    set_window_name();

    if (sys_getdspobjdspstate(m_owner))
        compile_dsp(vec_size, sampling_rate, true);

    // Finish loading (which fires loadbang and sets the valid flag)

    return load_finished(load_error::none, saved_loadupdate);
}

patch_slot::load_error patch_slot::load_finished(load_error error, short saved_loadupdate)
{
    loadbang_resume();
    dsp_setloadupdate(saved_loadupdate);
    m_valid = error == load_error::none ? true : false;
    if (!m_valid)
        free_patch();
    return error;
}

// Messages

void patch_slot::message(long inlet, t_symbol *msg, long argc, t_atom *argv)
{
    if (inlet < 0 || inlet >= get_num_ins())
        return;

    for (std::vector<void *>::iterator it = m_in_table[inlet].begin(); it != m_in_table[inlet].end(); it++)
        outlet_anything(*it, msg, static_cast<short>(argc), argv);
}

// Processing and DSP

bool patch_slot::process(void **outputs)
{
    if (m_dspchain && m_valid && m_on)
    {
        m_outputs = outputs;
        dspchain_tick(m_dspchain);
        return true;
    }

    return false;
}

void patch_slot::compile_dsp(long vec_size, long sampling_rate, bool force_when_invalid)
{
    if (m_valid || force_when_invalid)
    {
        // Free the old dspchain

        object_free(reinterpret_cast<t_object *>(m_dspchain));

        // Recompile

        m_dspchain = dspchain_compile(m_patch, vec_size, sampling_rate);

        // This won't work for gen~ the first time round, so we have to check and compile again
        // Hopefully c74 will sort it, but this code needs to remain for old versions of Max

        if (m_dspchain && m_dspchain->c_broken)
        {
            object_free(reinterpret_cast<t_object *>(m_dspchain));

            m_dspchain = dspchain_compile(m_patch, vec_size, sampling_rate);
        }
    }
}

// State

void patch_slot::set_on(bool on)
{
    m_on = on;

    if (get_patch())
    {
        for (auto it = m_state_listeners.begin(); it != m_state_listeners.end(); it++)
            object_method((*it), gensym("changeon"));
    }
}

void patch_slot::set_busy(bool busy)
{
    m_busy = busy;

    if (get_patch())
    {
        for (auto it = m_state_listeners.begin(); it != m_state_listeners.end(); it++)
            object_method((*it), gensym("changebusy"));
    }
}

// Listeners

void patch_slot::register_listener(t_object *listener)
{
    m_state_listeners.push_back(listener);
}

void patch_slot::unregister_listener(t_object *listener)
{
    auto &v = m_state_listeners;
    v.erase(std::remove(v.begin(), v.end(), listener), v.end());
}

// Window management

void patch_slot::set_window_name()
{
    constexpr size_t idx_length = 16;
    char index_string[idx_length];

    snprintf(index_string, idx_length, " (%lld)", static_cast<long long>(m_user_index));

    std::string window_name = std::string(m_name);
    window_name.append(index_string);
    jpatcher_set_title(m_patch, gensym(window_name.c_str()));
}

void patch_slot::open_window() const
{
    if (m_patch)
        object_method(m_patch, gensym("front"));
}

void patch_slot::close_window() const
{
    if (m_patch)
        object_method(m_patch, gensym("wclose"));
}

// Error strings

const char *patch_slot::get_error(load_error error)
{
    switch (error)
    {
        case load_error::file_not_found:    return "file not found";
        case load_error::nothing_loaded:    return "nothing was loaded";
        case load_error::not_patcher:       return "file is not a patcher";
        default:                            return "";
    }
}

// Inlet linking and unlinking

void patch_slot::handle_io(t_patcher *p, const char *type, long max_index, std::function<void(IO *, long)> func)
{
    t_symbol *ps_io = gensym(type);

    for (t_box *b = jpatcher_get_firstobject(p); b; b = jbox_get_nextobject(b))
    {
        if (jbox_get_maxclass(b) == ps_io)
        {
            IO *io = reinterpret_cast<IO *>(jbox_get_object(b));
            long index = io->s_index - 1;
            if (index >= 0 && index < max_index)
                func(io, index);
        }
    }
}

void patch_slot::find_ins(t_patcher *p)
{
    auto store = [&](IO *io, long index) { m_in_table[index].push_back(io->s_obj.o_outlet); };

    handle_io(p, "in", get_num_ins(), store);
}

void patch_slot::link_outlets(t_patcher *p)
{
    auto link = [&](IO *io, long index) { outlet_add(io->s_outlet, (*m_out_table)[index]); };

    handle_io(p, "out", get_num_outs(), link);
}

void patch_slot::unlink_outlets(t_patcher *p)
{
    auto unlink = [&](IO *io, long index) { outlet_rm(io->s_outlet, (*m_out_table)[index]); };

    handle_io(p, "out", get_num_outs(), unlink);
}

// Free patch

void patch_slot::free_patch()
{
    object_free(reinterpret_cast<t_object *>(m_dspchain));

    if (m_patch)
    {
        // Remove pointers to ins, unlink outlets, close window and free the patch

        for (std::vector<std::vector<void *>>::iterator it = m_in_table.begin(); it != m_in_table.end(); it++)
            it->clear();
        patcher_traverse<&patch_slot::unlink_outlets>(m_patch, nullptr, m_owner);
        close_window();

        // Defer patch deletion for speed and safety

        defer(m_patch, (method) delete_patch, nullptr, 0, nullptr);
    }

    m_state_listeners.clear();
    m_patch = nullptr;
    m_dspchain = nullptr;
}
