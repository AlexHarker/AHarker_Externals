
#ifndef _PATCHSLOT_H_
#define _PATCHSLOT_H_

#include <ext.h>
#include <z_dsp.h>

#include <functional>
#include <string>
#include <vector>

#include <AH_Locks.hpp>


// patch_slot Class

class patch_slot
{
    // Generic in/out structure

    struct IO
    {
        t_object s_obj;

        long s_index;
        void *s_outlet;
    };

    enum { MAX_ARGS = 16 };

public:

    enum LoadError { kNone, kFileNotFound, kNothingLoaded, kNotPatcher };

    patch_slot(t_object *owner, t_object *parent, t_atom_long index, long num_ins, std::vector<void *> *out_table)
    : m_patch(nullptr), m_dspchain(nullptr), m_path_symbol(nullptr)
    , m_path(0), m_user_index(index), m_argc(0)
    , m_valid(false), m_on(false), m_busy(false)
    , m_outputs(nullptr), m_out_table(out_table)
    , m_owner(owner), m_parent(parent)
    {
        m_in_table.resize(num_ins);
    }

    ~patch_slot();

    LoadError load(t_symbol *path, long argc, t_atom *argv, long vec_size, long sampling_rate);
    LoadError load(long vec_size, long sampling_rate, bool initialise);

    void message(long inlet, t_symbol *msg, long argc, t_atom *argv);

    void compile_dsp(long vecSize, long sampling_rate, bool force_when_invalid);
    bool process(void **outputs);

    // Getters

    t_patcher *get_patch() const        { return m_patch; }
    t_atom_long get_user_index() const  { return m_user_index; }
    bool get_valid() const              { return m_valid; }
    bool get_on() const                 { return m_on; }
    bool get_busy() const               { return m_busy; }
    void ***get_output_handle()         { return &m_outputs; }

    // Setters

    void set_on(bool on);
    void set_busy(bool busy);
    void set_invalid()          { m_valid = false; }

    // Listeners

    void register_listener(t_object *listener);
    void unregister_listener(t_object *listener);

    // Number of ins and outs

    long get_num_ins() const    { return m_in_table.size(); }
    long get_num_outs() const   { return m_out_table->size(); }

    // Windows Management

    void open_window() const;
    void close_window() const;

    // Error string

    static const char *get_error(LoadError error);

private:

    template <void (patch_slot::*Method)(t_patcher *p)>
    int patcher_traverse(t_patcher *p, void *arg, t_object *owner);

    LoadError load_finished(LoadError error, short saved_loadupdate);

    void set_window_name();
    void free_patch();

    void handle_io(t_patcher *p, const char *type, long max_index, std::function<void(IO*, long)> func);

    void find_ins(t_patcher *p);
    void link_outlets(t_patcher *p);
    void unlink_outlets(t_patcher *p);

    // Patch and variables / dspchain

    t_patcher *m_patch;
    t_dspchain *m_dspchain;
    t_symbol *m_path_symbol;
    std::string m_name;
    short m_path;
    long m_user_index;

    // Arguments (stored in case of reload / update)

    long m_argc;
    t_atom m_argv[MAX_ARGS];

    // Flags

    bool m_valid;
    bool m_on;
    bool m_busy;

    // Pointer to Array of Audio Out Buffers (which are thread dependent)

    void **m_outputs;

    // Inlet and Outlet Communication

    std::vector<std::vector<void *>> m_in_table;
    std::vector<void *> *m_out_table;

    // State Listener Objects (need notifying)

    std::vector<t_object *> m_state_listeners;

    // Owner

    t_object *m_owner;
    t_patcher *m_parent;
};


// Class with threading additions

class threaded_patch_slot : public patch_slot
{

public:

    threaded_patch_slot(t_object *owner, t_patcher *parent, t_atom_long index, long numIns, std::vector<void *> *out_table)
    : patch_slot(owner, parent, index, numIns, out_table), m_thread_current(0), m_thread_request(0) {}

    void request_thread(long thread)    { m_thread_request = thread; }
    void update_thread()                { m_thread_current = m_thread_request; }
    void reset_processed()              { m_processing_lock.release(); }

    bool process_if_unprocessed(void **outputs)
    {
        return m_processing_lock.attempt() ? process(outputs) : false;
    }

    bool process_if_thread_matches(void **outputs, long thread, long n_threads)
    {
        return ((m_thread_current % n_threads) == thread) ? process(outputs) : false;
    }

private:

    // Threading Variables

    thread_lock m_processing_lock;
    long m_thread_current;
    long m_thread_request;
};

#endif  /* _PATCHSLOT_H_ */
