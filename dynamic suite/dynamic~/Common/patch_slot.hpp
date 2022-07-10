
#ifndef _PATCHSLOT_HPP_
#define _PATCHSLOT_HPP_

#include <ext.h>
#include <z_dsp.h>

#include <functional>
#include <string>
#include <vector>

#include <AH_Locks.hpp>


class patch_slot
{
    // Generic in/out structure

    struct IO
    {
        t_object s_obj;

        long s_index;
        void *s_outlet;
    };

    enum { max_num_args = 16 };

public:

    static constexpr long max_args() { return max_num_args; }
    
    enum class load_error
    {
        none,
        file_not_found,
        nothing_loaded,
        not_patcher
    };

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

    load_error load(t_symbol *path, long argc, t_atom *argv, long vec_size, long sampling_rate);
    load_error load(long vec_size, long sampling_rate, bool initialise);

    void message(long inlet, t_symbol *msg, long argc, t_atom *argv);

    void compile_dsp(long vec_size, long sampling_rate, bool force_when_invalid);
    bool process(double **outputs);

    // Getters

    t_patcher *get_patch() const        { return m_patch; }
    t_atom_long get_user_index() const  { return m_user_index; }
    bool get_valid() const              { return m_valid; }
    bool get_on() const                 { return m_on; }
    bool get_busy() const               { return m_busy; }
    double ***get_output_handle()       { return &m_outputs; }

    // Setters

    void set_on(bool on);
    void set_busy(bool busy);
    void set_invalid()          { m_valid = false; }

    // Listeners

    void register_listener(t_object *listener);
    void unregister_listener(t_object *listener);

    // Number of ins and outs

    long get_num_ins() const    { return static_cast<long>(m_in_table.size()); }
    long get_num_outs() const   { return static_cast<long>(m_out_table->size()); }

    // Windows management

    void open_window() const;
    void close_window() const;

    // Error string

    static const char *get_error(load_error error);

private:

    template <void (patch_slot::*Method)(t_patcher *p)>
    int patcher_traverse(t_patcher *p, void *arg, t_object *owner);

    load_error load_finished(load_error error, short saved_loadupdate);

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
    t_atom_long m_user_index;

    // Arguments (stored in case of reload / update)

    long m_argc;
    t_atom m_argv[max_num_args];

    // Flags

    bool m_valid;
    bool m_on;
    bool m_busy;

    // Pointer to array of audio out buffers (which are thread dependent)

    double **m_outputs;

    // Inlet and outlet communication

    std::vector<std::vector<void *>> m_in_table;
    std::vector<void *> *m_out_table;

    // State listener objects (need notifying)

    std::vector<t_object *> m_state_listeners;

    // Owner

    t_object *m_owner;
    t_patcher *m_parent;
};


// Class with threading additions

class threaded_patch_slot : public patch_slot
{

public:

    threaded_patch_slot(t_object *owner, t_patcher *parent, t_atom_long index, long num_ins, std::vector<void *> *out_table)
    : patch_slot(owner, parent, index, num_ins, out_table), m_thread_current(0), m_thread_request(0) {}

    void request_thread(t_atom_long thread)     { m_thread_request = thread; }
    void update_thread()                        { m_thread_current = m_thread_request; }
    void reset_processed()                      { m_processing_lock.release(); }

    bool process_if_unprocessed(double **outputs)
    {
        return m_processing_lock.attempt() ? process(outputs) : false;
    }

    bool process_if_thread_matches(double **outputs, t_atom_long thread, t_atom_long n_threads)
    {
        return ((m_thread_current % n_threads) == thread) ? process(outputs) : false;
    }

private:

    // Threading

    thread_lock m_processing_lock;
    t_atom_long m_thread_current;
    t_atom_long m_thread_request;
};

#endif /* _PATCHSLOT_HPP_ */
