
#ifndef DYNAMIC_HOST_H
#define DYNAMIC_HOST_H

// These are the common handlers for dynamic host objects
// The object is expected to have a member called patch_set

template <typename T>
struct DynamicHost
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
        x->patch_set->messageBang();
    }
    
    static void msgint(T *x, t_atom_long n)
    {
        x->patch_set->messageInt(n);
    }
    
    static void msgfloat(T *x, double f)
    {
        x->patch_set->messageFloat(f);
    }
    
    static void msglist(T *x, t_symbol *s, long argc, t_atom *argv)
    {
        x->patch_set->messageAnything(s, argc, argv);
    }
    
    static void msganything(T *x, t_symbol *s, long argc, t_atom *argv)
    {
        x->patch_set->messageAnything(s, argc, argv);
    }
    
    // Voice targeting
    
    static void target(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        x->patch_set->target(argc, argv);
    }
    
    static void targetfree(T *x, t_symbol *msg, long argc, t_atom *argv)
    {
        x->patch_set->targetFree(argc, argv);
    }
    
    // Patcher windows
    
    static void dblclick(T *x)
    {
        for (long i = 1; i <= x->patch_set->size(); i++)
            if (x->patch_set->openWindow(i))
                break;
    }
    
    static void open(T *x, t_atom_long index)
    {
        x->patch_set->openWindow(index);
    }
    
    static void wclose(T *x, t_atom_long index)
    {
        x->patch_set->closeWindow(index);
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
        return x->patch_set->patchIndex(p);
    }
    
    // Parent / Child Communication - Routines for owned objects to query the parent
    
    // Loading Index
    
    static void loading_index(T *x, t_atom_long *index)
    {
        *index = x->patch_set->getLoadingIndex();
    }
    
    // Listeners
    
    static void register_listener(T *x, t_ptr_uint idx, t_object *listener)
    {
        x->patch_set->registerListener(idx, listener);
    }
    
    static void unregister_listener(T *x, t_ptr_uint idx, t_object *listener)
    {
        x->patch_set->unregisterListener(idx, listener);
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
    
    static void query_sigouts(T *x, long index, void ****out_handle)
    {
        *out_handle = x->patch_set->getOutputHandle(index);
    }
    
    // State
    
    static void client_get_patch_on(T *x, t_ptr_uint idx, t_atom_long *state)
    {
        *state = x->patch_set->getOn(idx);
    }
    
    static void client_get_patch_busy(T *x, t_ptr_uint idx, t_atom_long *state)
    {
        *state = x->patch_set->getBusy(idx);
    }
    
    static void client_set_patch_on(T *x, t_ptr_uint idx, t_ptr_uint state)
    {
        x->patch_set->setOn(idx, state);
    }
    
    static void client_set_patch_busy(T *x, t_ptr_uint idx, t_ptr_uint state)
    {
        x->patch_set->setBusy(idx, state);
    }
};

#endif /* DYNAMIC_HOST_H */

