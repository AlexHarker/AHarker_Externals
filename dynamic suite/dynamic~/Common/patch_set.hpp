
#ifndef _PATCHSET_HPP_
#define _PATCHSET_HPP_

#include <ext.h>
#include <jpatcher_api.h>
#include <ext_dictionary.h>
#include <ext_dictobj.h>

#include <memory>
#include <vector>

#include <AH_Containers.hpp>

#include "patch_slot.hpp"

// Attribute helper for dspchains

static t_max_err patchset_get_ownsdspchain(t_object *pv, t_object *attr, long *argc, t_atom **argv)
{
    char alloc = false;

    if (atom_alloc(argc, argv, &alloc))
        return MAX_ERR_GENERIC;

    // This prevents getdspchain on child patchers from walking up past this object

    atom_setlong(*argv, 1);
    return MAX_ERR_NONE;
}

// Class

// N.B. index counting is from 1 (0 means all or out of range)

template <class T>
class patch_set
{
    typedef T* (patch_set::*get_slot)(t_atom_long index);

protected:
    
    using list_type = typename threadsafe_pointer_list<T>::list_type;
    using slot_access = typename threadsafe_pointer_list<T>::access;
    
public:

    // Deferred helpers

    static void deferred_load(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        patch_set *set = reinterpret_cast<patch_set *>(atom_getobj(argv + 0));
        t_atom_long index = atom_getlong(argv + 1);
        long vec_size = static_cast<long>(atom_getlong(argv + 2));
        long sampling_rate = static_cast<long>(atom_getlong(argv + 3));

        set->do_load(index, s, argc - 4, argv + 4, vec_size, sampling_rate);
    }

    static void do_delete(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        patch_set *set = reinterpret_cast<patch_set *>(atom_getobj(argv));
        set->m_slots.remove(index_from_atom(argv + 1));
    }

    static void do_open(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        auto slot = slot_from_atom(argv);
        
        if (slot)
            slot->open_window();
    }

    static void do_close(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        auto slot = slot_from_atom(argv);
        
        if (slot)
            slot->close_window();
    }

    // Constructor

    patch_set(t_object *x, t_patcher *parent, long num_ins, long num_outs, void **outs)
    : m_owner(x), m_parent(parent), m_loading_index(0), m_target_index(0), m_num_ins(num_ins)
    {
        m_out_table.assign(outs, outs + num_outs);
    }

    // Size

    size_t num_patches() const { return m_slots.members(); }
    
    //long size() const { return static_cast<long>(m_slots.size()); }

    // Load / Remove / Clear

    t_atom_long load(t_atom_long index, t_symbol *path, long argc, t_atom *argv, long vec_size, long sampling_rate)
    {
        std::vector<t_atom> deferred_args(argc + 4);

        // FIX - some kind of high value check for user specified index?

        // Find a free patch if no index is given

        if (index < 1)
        {
            for (index = 1; index <= size(); index++)
                if (!m_slots[index - 1])
                    break;
        }

        // Set arguments

        atom_setobj(deferred_args.data() + 0, this);
        atom_setlong(deferred_args.data() + 1, index);
        atom_setlong(deferred_args.data() + 2, vec_size);
        atom_setlong(deferred_args.data() + 3, sampling_rate);

        for (long i = 0; i < argc; i++)
            deferred_args[i + 4] = argv[i];

        defer(m_owner, (method) deferred_load, path, static_cast<short>(argc + 4), deferred_args.data());

        return index;
    }

    void remove(t_atom_long index)
    {
        slot_access slots(m_slots);
        
        if (slot_exists(slots(), index))
            slots()[index - 1]->set_invalid();
            
        if (!defer_slot_action(slots(), index, (method) do_delete))
            object_error(m_owner, "no patch found in slot %ld", index);
    }

    void clear()
    {
        m_slots.clear();
    }

    // Message communication

    void message_bang()
    {
        send_message(gensym("bang"), 0, nullptr);
    }

    void message_int(t_atom_long n)
    {
        t_atom n_atom;
        atom_setlong(&n_atom, n);
        send_message(gensym("int"), 1, &n_atom);
    }

    void message_float(double f)
    {
        t_atom f_atom;
        atom_setfloat(&f_atom, f);
        send_message(gensym("float"), 1, &f_atom);
    }

    void message_anything(t_symbol *s, long argc, t_atom *argv)
    {
        send_message(s, argc, argv);
    }

    // Target methods

    void target(long argc, t_atom *argv)
    {
        slot_access slots(m_slots);
        
        t_atom_long index = argc ? atom_getlong(argv) : 0;

        m_target_index = slot_exists(slots(), index) ? index : -1;
    }

    void target_free(long argc, t_atom *argv)
    {
        slot_access slots(m_slots);
        auto slot_list = slots();
        
        t_atom_long max_slot = slot_list.size();
        t_atom_long lo = 0;
        t_atom_long hi = max_slot;

        // Get and clip inputs

        t_atom_long in1 = argc > 0 ? std::min(std::max(atom_getlong(argv + 0), t_atom_long(1)), max_slot) : 1;
        t_atom_long in2 = argc > 1 ? std::min(std::max(atom_getlong(argv + 1), t_atom_long(1)), max_slot) : 1;

        // Load arguments

        if (argc && max_slot)
        {
            if (argc > 1)
            {
                lo = std::min(in1, in2) - 1;
                hi = std::max(in1, in2);
            }
            else
                hi = in1;
        }

        // Search for a free voice

        for (t_atom_long i = lo; i < hi; i++)
        {
            if (slot_list[i] && slot_list[i]->get_valid() && !slot_list[i]->get_busy())
            {
                m_target_index = i + 1;
                return;
            }
        }

        m_target_index = -1;
    }

    // Process and DSP

    bool process(t_atom_long index, void **outputs)
    {
        return slot_action_result(&T::process, index, outputs);
    }

    void compile_dsp(long vec_size, long sampling_rate)
    {
        for_all_slots(&T::compile_dsp, vec_size, sampling_rate, false);
    }

    // Window management

    void open_window(t_atom_long index)
    {
        slot_access slots(m_slots);

        defer_slot_action(slots(), index, (method) do_open);
    }
    
    void open_first_window()
    {
        slot_access slots(m_slots);
        
        for (t_atom_long i = 1; i <= slots().size(); i++)
            if (defer_slot_action(slots(), i, (method) do_open))
                break;
    }

    void close_window(t_atom_long index)
    {
        slot_access slots(m_slots);

        if (!index)
        {
            for (t_atom_long i = 1; i <= slots().size(); i++)
                defer_slot_action(slots(), i, (method) do_close);
        }
        else
            defer_slot_action(slots(), index, (method) do_close);
    }

    // Listeners

    void register_listener(t_ptr_int index, t_object *listener)
    {
        slot_action(&T::register_listener, index, listener);
    }

    void unregister_listener(t_ptr_int index, t_object *listener)
    {
        slot_action(&T::unregister_listener, index, listener);
    }

    // Queries

    void ***get_output_handle(t_ptr_int index)
    {
        slot_access slots(m_slots);
        
        return slot_exists(slots(), index) ? slots()[index - 1]->get_output_handle() : nullptr;
    }

    bool get_on(t_ptr_int index)
    {
        return slot_action_result(&T::get_on, index);
    }

    bool get_busy(t_ptr_int index)
    {
        return slot_action_result(&T::get_busy, index);
    }

    t_atom_long get_loading_index()
    {
        return m_loading_index;
    }

    void set_on(t_ptr_int index, t_ptr_int state)
    {
        slot_action(&T::set_on, index, state ? true : false);
    }

    void set_busy(t_ptr_int index, t_ptr_int state)
    {
        slot_action(&T::set_busy, index, state ? true : false);
    }

    // Update

    void update(t_patcher *p, long vec_size, long sampling_rate)
    {
        slot_access slots(m_slots);
        
        // Reload the patcher when it's updated

        for (auto it = slots().begin(); it != slots().end(); it++)
        {
            if ((*it) && (*it)->get_patch() == p)
            {
                (*it)->load(vec_size, sampling_rate, false);
                break;
            }
        }
    }

    // Get patch index from a pointer to the patch (used also by the max patch traversal system)

    t_atom_long patch_index(t_patcher *p)
    {
        slot_access slots(m_slots);
        
        for (auto it = slots().begin(); it != slots().end(); it++)
            if ((*it) && (*it)->get_patch() == p)
                return (*it)->get_user_index();

        return -1;
    }

    // Get patch for subpatcher reporting

    t_patcher* subpatch(long index, void *arg)
    {
        slot_access slots(m_slots);

        // Report subpatchers if requested by an object that is not a dspchain

        if ((t_ptr_uint) arg > 1)
            if (!NOGOOD(arg))
                if (ob_sym(arg) == gensym("dspchain"))
                    return nullptr;

        // Loop over hosted patches to find the next one

        long count = 0;

        for (auto it = slots().begin(); it != slots().end(); it++)
            if ((*it) && (*it)->get_patch() && count++ == index)
                return (*it)->get_patch();

        return nullptr;
    }

protected:

    static bool slot_exists(list_type& slots, t_atom_long index)
    {
        return index >= 1 && index <= slots.size() && slots[index - 1];
    }

    void do_load(t_atom_long index, t_symbol *path, long argc, t_atom *argv, long vec_size, long sampling_rate)
    {
        // Load into the specified slot

        m_loading_index = index;
        m_slots.add(new T(m_owner, m_parent, index, m_num_ins, &m_out_table), index - 1);
        
        slot_access slots(m_slots);
        
        auto error = slots()[index - 1]->load(path, argc, argv, vec_size, sampling_rate);
        m_loading_index = 0;

        // If there is an error report and empty the slot

        if (error != T::load_error::none)
        {
            m_slots.remove(index - 1);
            object_error(m_owner, "error trying to load patch %s - %s", path->s_name, T::get_error(error));
        }
    }

    void send_message(t_symbol *msg, long argc, t_atom *argv)
    {
        long inlet = proxy_getinlet(m_owner);

        if (m_target_index == 0)
            for_all_slots(&T::message, inlet, msg, argc, argv);
        else
            slot_action(&T::message, m_target_index, inlet, msg, argc, argv);
    }

    bool defer_slot_action(list_type& list, t_atom_long index, method action)
    {
        if (!slot_exists(list, index))
            return false;

        t_atom a[2];
        atom_setobj(a + 0, this);
        atom_setlong(a + 1, index);
        defer(m_owner, action, 0L, 1, a);
        return true;
    }

    static t_atom_long index_from_atom(t_atom *argv)
    {
        return atom_getlong(argv);
    }
    
    static T *slot_from_atom(t_atom *argv)
    {
        patch_set *set = reinterpret_cast<patch_set *>(atom_getobj(argv + 0));
        auto index = atom_getlong(argv + 1);
        
        return slot_access(set->m_slots)()[index];
    }

    template <typename Method, typename ...Args>
    void slot_action(Method method, t_atom_long index, Args...args)
    {
        slot_access slots(m_slots);
        auto list = slots();
        
        if (slot_exists(list, index))
            (*list[index - 1].*method)(args...);
    }

    template <typename Method, typename ...Args>
    bool slot_action_result(Method method, t_atom_long index, Args...args)
    {
        slot_access slots(m_slots);
        auto list = slots();
        
        if (!slot_exists(list, index))
            return false;

        return (*list[index - 1].*method)(args...);
    }

    template <typename Method, typename ...Args>
    void for_all_slots(Method method, Args...args)
    {
        slot_access slots(m_slots);
        
        for (auto it = slots().begin(); it != slots().end(); it++)
            if (*it) ((**it).*method)(args...);
    }

    // Data

    t_object *m_owner;
    t_patcher *m_parent;

    t_atom_long m_loading_index;
    t_atom_long m_target_index;
    long m_num_ins;

    std::vector<void *> m_out_table;
    threadsafe_pointer_list<T> m_slots;
};


// Class with threading additions

struct threaded_patch_set : public patch_set<threaded_patch_slot>
{
    threaded_patch_set(t_object *x, t_patcher *parent, long num_ins, long num_outs, void **outs)
    : patch_set(x, parent, num_ins, num_outs, outs) {}

    void process_if_thread_matches(void **outputs, t_atom_long thread, t_atom_long n_threads)
    {
        slot_access slots(m_slots);
        
        for (auto it = slots().begin(); it != slots().end(); it++)
            if (*it) (*it)->process_if_thread_matches(outputs, thread, n_threads);
    }

    bool process_if_unprocessed(t_atom_long index, void **outputs)
    {
        return slot_action_result(&threaded_patch_slot::process_if_unprocessed, index, outputs);
    }

    void request_thread(t_atom_long index, t_atom_long thread)
    {
        slot_action(&::threaded_patch_slot::request_thread, index, thread);
    }

    void reset_processed()   { for_all_slots(&threaded_patch_slot::reset_processed); }
    void update_threads()    { for_all_slots(&threaded_patch_slot::update_thread); }
};

#endif /* _PATCHSET_HPP_ */
