
#ifndef _DYNAMIC_HOST_HPP_
#define _DYNAMIC_HOST_HPP_

// These are the common handlers for dynamic host objects
// The object is expected to have a member called patch_set

template <typename T>
struct dynamic_host
{
    // Removing patches

    static void deletepatch(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        x->patch_set->remove(atom_getlong(argv));
    }

    static void clear(T *x)
    {
        x->patch_set->clear();
    }

    // Messages in passed on to the patcher via the "in" objects

    static void msgbang(T *x)
    {
        x->patch_set->message_bang();
    }

    static void msgint(T *x, t_atom_long n)
    {
        x->patch_set->message_int(n);
    }

    static void msgfloat(T *x, double f)
    {
        x->patch_set->message_float(f);
    }

    static void msglist(T *x, t_symbol *s, long argc, t_atom *argv)
    {
        x->patch_set->message_anything(s, argc, argv);
    }

    static void msganything(T *x, t_symbol *s, long argc, t_atom *argv)
    {
        x->patch_set->message_anything(s, argc, argv);
    }

    // Voice targeting

    static void target(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        x->patch_set->target(argc, argv);
    }

    static void targetfree(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        x->patch_set->target_free(argc, argv);
    }

    // Patcher windows

    static void dblclick(T *x)
    {
        for (long i = 1; i <= x->patch_set->size(); i++)
            if (x->patch_set->open_window(i))
                break;
    }

    static void open(T *x, t_atom_long index)
    {
        x->patch_set->open_window(index);
    }

    static void wclose(T *x, t_atom_long index)
    {
        x->patch_set->close_window(index);
    }

    // Patcher utilities (deal with various updating and necessary behind the scenes state stuff)

    static void pupdate(T *x, void *b, t_patcher *p)
    {
        x->patch_set->update(p, x->last_vec_size, x->last_samp_rate);
    }

    static void *subpatcher(T *x, long index, void *arg)
    {
        return x->patch_set->subpatch(index, arg);
    }

    static void parentpatcher(T *x, t_patcher **parent)
    {
        *parent = x->parent_patch;
    }

    static t_atom_long getindex(T *x, t_patcher *p)
    {
        return x->patch_set->patch_index(p);
    }

    // Parent / Child Communication - Routines for owned objects to query the parent

    // Loading Index

    static void loading_index(T *x, t_atom_long *index)
    {
        *index = x->patch_set->get_loading_index();
    }

    // Listeners

    static void register_listener(T *x, t_ptr_uint idx, t_object *listener)
    {
        x->patch_set->register_listener(idx, listener);
    }

    static void unregister_listener(T *x, t_ptr_uint idx, t_object *listener)
    {
        x->patch_set->unregister_listener(idx, listener);
    }

    // Signals

    static void query_num_sigins(T *x, long *num_sig_ins)
    {
        *num_sig_ins = x->num_sig_ins;
    }

    static void query_num_sigouts(T *x, long *num_sig_outs)
    {
        *num_sig_outs = x->num_sig_outs;
    }

    static void query_sigins(T *x, void ***sig_ins)
    {
        *sig_ins = x->sig_ins;
    }

    static void query_sigouts(T *x, t_ptr_uint idx, void ****out_handle)
    {
        *out_handle = x->patch_set->get_output_handle(idx);
    }

    // State

    static void client_get_patch_on(T *x, t_ptr_uint idx, t_atom_long *state)
    {
        *state = x->patch_set->get_on(idx);
    }

    static void client_get_patch_busy(T *x, t_ptr_uint idx, t_atom_long *state)
    {
        *state = x->patch_set->get_busy(idx);
    }

    static void client_set_patch_on(T *x, t_ptr_uint idx, t_ptr_uint state)
    {
        x->patch_set->set_on(idx, state);
    }

    static void client_set_patch_busy(T *x, t_ptr_uint idx, t_ptr_uint state)
    {
        x->patch_set->set_busy(idx, state);
    }
};

#endif /* _DYNAMIC_HOST_HPP_ */
