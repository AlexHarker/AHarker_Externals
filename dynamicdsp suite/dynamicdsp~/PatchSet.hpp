
#ifndef _PATCHSET_H_
#define _PATCHSET_H_

#include <ext.h>
#include <jpatcher_api.h>
#include <ext_dictionary.h>
#include <ext_dictobj.h>

#include <memory>
#include <vector>


// Attribute Helper for dspchains

static t_max_err patchset_get_ownsdspchain(t_object *pv, t_object *attr, long *argc, t_atom **argv)
{
    char alloc = false;
    
    if (atom_alloc(argc, argv, &alloc))
        return MAX_ERR_GENERIC;
    
    // This prevents getdspchain on child patchers from walking up past this object
    
    atom_setlong(*argv, 1);
    return MAX_ERR_NONE;
}

// Main class

// N.B. index counting is from 1 (0 means all or out of range)

template <class SlotClass>
class PatchSet
{
    typedef SlotClass* (PatchSet::*GetSlot)(t_atom_long index);
    
public:
    
    // Deferred helpers
    
    static void deferredLoad(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        PatchSet *set = reinterpret_cast<PatchSet *>(atom_getobj(argv + 0));
        t_atom_long index = atom_getlong(argv + 1);
        long vecSize = static_cast<long>(atom_getlong(argv + 2));
        long samplingRate = static_cast<long>(atom_getlong(argv + 3));

        set->doLoad(index, s, argc - 4, argv + 4, vecSize, samplingRate);
    }
    
    static void doDelete(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        delete slotFromAtom(argv);
    }
    
    static void doOpen(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        slotFromAtom(argv)->openWindow();
    }
    
    static void doClose(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        slotFromAtom(argv)->closeWindow();
    }
    
    // Constructor
    
    PatchSet(t_object *x, t_patcher *parent, long numIns, long numOuts, void **outs)
    : mOwner(x), mParent(parent), mTargetIndex(0), mNumIns(numIns)
    {
        mOutTable.assign(outs, outs + numOuts);
    }
    
    // Size
    
    long size() const { return mSlots.size(); }
    
    // Load / Remove / Clear
    
    long load(t_atom_long index, t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate)
    {
        std::vector<t_atom> deferredArgs(argc + 4);

        // FIX - some kind of high value check for user specified index?

        // Find a free patch if no index is given
        
        if (index < 1)
        {
            for (index = 1; index <= size(); index++)
                if (!mSlots[index - 1])
                    break;
        }

        // Set arguments
        
        atom_setobj(deferredArgs.data() + 0, this);
        atom_setlong(deferredArgs.data() + 1, index);
        atom_setlong(deferredArgs.data() + 2, vecSize);
        atom_setlong(deferredArgs.data() + 3, samplingRate);
        
        for (long i = 0; i < argc; i++)
            deferredArgs[i + 4] = argv[i];
        
        defer(mOwner, (method) deferredLoad, path, argc + 4, deferredArgs.data());
    
        return index;
    }
    
    void remove(t_atom_long index)
    {
        if (!deferSlotAction(index, (method) doDelete, &PatchSet::release))
            object_error(mOwner, "no patch found in slot %ld", index);
    }
    
    void clear()
    {
        for (t_atom_long i = 1; i <= size(); i++)
            deferSlotAction(i, (method) doDelete, &PatchSet::release);
    }
    
    // Message Communication
    
    void messageBang()
    {
        sendMessage(gensym("bang"), 0, nullptr);
    }
    
    void messageInt(t_atom_long n)
    {
        t_atom n_atom;
        atom_setlong(&n_atom, n);
        sendMessage(gensym("int"), 1, &n_atom);
    }
    
    void messageFloat(double f)
    {
        t_atom f_atom;
        atom_setfloat(&f_atom, f);
        sendMessage(gensym("float"), 1, &f_atom);
    }
    
    void messageAnything(t_symbol *s, long argc, t_atom *argv)
    {
        sendMessage(s, argc, argv);
    }
    
    // Target Methods
    
    void target(long argc, t_atom *argv)
    {
        t_atom_long index = argc ? atom_getlong(argv) : 0;
        
        mTargetIndex = (index >= 0 && index <= mSlots.size()) ? index : -1;
    }
    
    void targetFree(long argc, t_atom *argv)
    {
        t_atom_long maxSlot = size();
        t_atom_long lo = 0;
        t_atom_long hi = maxSlot;
        
        // Get and clip inputs
        
        t_atom_long in1 = argc > 0 ? std::min(std::max(in1, t_atom_long(1)), maxSlot) : 1;
        t_atom_long in2 = argc > 1 ? std::min(std::max(in2, t_atom_long(1)), maxSlot) : 1;
        
        // Load arguments
        
        if (argc && maxSlot)
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
            if (mSlots[i] && mSlots[i]->getValid() && !mSlots[i]->getBusy())
            {
                mTargetIndex = i + 1;
                return;
            }
        }
        
        mTargetIndex = -1;
    }
    
    // Process and DSP
    
    bool process(t_atom_long index, void **outputs)
    {
        return slotActionResult(&SlotClass::process, index, outputs);
    }
    
    void compileDSP(long vecSize, long samplingRate)
    {
        forAllSlots(&SlotClass::compileDSP, vecSize, samplingRate, false);
    }

    // Window Management
    
    bool openWindow(t_atom_long index)
    {
        return deferSlotAction(index, (method) doOpen, &PatchSet::get);
    }
    
    bool closeWindow(t_atom_long index)
    {
        if (!index)
        {
            for (t_atom_long i = 1; i <= size(); i++)
                deferSlotAction(i, (method) doClose, &PatchSet::get);
            
            return true;
        }
        else
            return deferSlotAction(index, (method) doClose, &PatchSet::get);
    }
    
    // Queries
    
    void *getOutputHandle(t_ptr_int index)
    {
        return userSlotExists(index) ? mSlots[index - 1]->getOutputsHandle() : nullptr;
    }
    
    bool getOn(t_ptr_int index)
    {
        return slotActionResult(&PatchSlot::getOn, index);
    }
    
    bool getBusy(t_ptr_int index)
    {
        return slotActionResult(&PatchSlot::getBusy, index);
    }
    
    void setOn(t_ptr_int index, t_ptr_int state)
    {
        slotAction(&PatchSlot::setOn, index, state ? true : false);
    }
    
    void setBusy(t_ptr_int index, t_ptr_int state)
    {
        slotAction(&PatchSlot::setBusy, index, state ? true : false);
    }
    
    // Update
    
    void update(t_patcher *p, long vecSize, long samplingRate)
    {
        // Reload the patcher when it's updated
        
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
        {
            if ((*it) && (*it)->getPatch() == p)
            {
                (*it)->load(vecSize, samplingRate, false);
                break;
            }
        }
    }
    
    // Get patch for subpatcher reporting
    
    t_patcher* subpatch(long index, void *arg, long *userIndex = nullptr)
    {
        // Report subpatchers if requested by an object that is not a dspchain
        
        // FIX - this looks wrong to me...
        
        if ((t_ptr_uint) arg > 1)
            if (!NOGOOD(arg))
                if (ob_sym(arg) == gensym("dspchain"))
                    return NULL;
        
        //  IS THIS CORRECT??
        
        // if ((t_ptr_uint) arg <= 1 || NOGOOD(arg) || ob_sym(arg) == gensym("dspchain"))
        // return NULL;
        
        // Loop over hosted patches to find the next one
        
        long count = 0;
        
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
        {
            if ((*it) && (*it)->getPatch())
            {
                if (userIndex)
                    *userIndex = (*it)->getUserIndex();
                if (count++ == index)
                    return (*it)->getPatch();
            }
        }
        
        if (userIndex)
            *userIndex = -1;
        
        return nullptr;
    }
    
protected:

    bool slotExists(t_atom_long index)
    {
        return index >= 1 && index <= mSlots.size() && mSlots[index - 1];
    }
    
    void doLoad(t_atom_long index, t_symbol *path, long argc, t_atom *argv, long vecSize, long samplingRate)
    {
        // Load into a new slot
        
        auto slot = std::unique_ptr<SlotClass>(new SlotClass(mOwner, mParent, mNumIns, &mOutTable));
        auto error = slot->load(index, path, argc, argv, vecSize, samplingRate);
        
        // Deal with error or retain in the specified slot
        
        if (error != PatchSlot::LoadError::kNone)
            object_error(mOwner, "error trying to load patch %s - %s", path->s_name, PatchSlot::getError(error));
        else
            mSlots.emplace(mSlots.begin() + index, std::move(slot));
    }
    
    void sendMessage(t_symbol *msg, long argc, t_atom *argv)
    {
        long inlet = proxy_getinlet(mOwner);
        
        if (mTargetIndex == 0)
            forAllSlots(&SlotClass::message, inlet, msg, argc, argv);
        else
            slotAction(&SlotClass::message, mTargetIndex, inlet, msg, argc, argv);
    }
    
    SlotClass *get(t_atom_long index)
    {
        return mSlots[index - 1].get();
    }
    
    SlotClass *release(t_atom_long index)
    {
        mSlots[index - 1]->setInvalid();
        return mSlots[index - 1].release();
    }
    
    bool deferSlotAction(t_atom_long index, method action, GetSlot getter)
    {
        if (!slotExists(index))
            return false;
        
        t_atom a;
        atom_setobj(&a, (this->*getter)(index));
        defer(mOwner, action, 0L, 1, &a);
        return true;
    }
    
    static SlotClass *slotFromAtom(t_atom *argv)
    {
        return reinterpret_cast<SlotClass *>(atom_getobj(argv));
    }
    
    template <typename Method, typename ...Args>
    void slotAction(Method method, t_atom_long index, Args...args)
    {
        if (slotExists(index))
            (*mSlots[index - 1].*method)(args...);
    }
    
    template <typename Method, typename ...Args>
    bool slotActionResult(Method method, t_atom_long index, Args...args)
    {
        if (!slotExists(index))
            return false;
        
        return (*mSlots[index - 1].*method)(args...);
    }

    template <typename Method, typename ...Args>
    void forAllSlots(Method method, Args...args)
    {
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
            if (*it) ((**it).*method)(args...);
    }
    
    // Data
   
    t_object *mOwner;
    t_patcher *mParent;
    long mTargetIndex;
    
    long mNumIns;
    
    std::vector<void *> mOutTable;
    std::vector<std::unique_ptr<SlotClass>> mSlots;
};


// Class with threading additions

struct ThreadedPatchSet : public PatchSet<ThreadedPatchSlot>
{
    ThreadedPatchSet(t_object *x, t_patcher *parent, long numIns, long numOuts, void **outs)
    : PatchSet(x, parent, numIns, numOuts, outs) {}
    
    bool processIfThreadMatches(t_atom_long index, void **outputs, long thread, long nThreads)
    {
        return slotActionResult(&ThreadedPatchSlot::processIfThreadMatches, index, outputs, thread, nThreads);
    }
    
    bool processIfUnprocessed(t_atom_long index, void **outputs)
    {
        return slotActionResult(&ThreadedPatchSlot::processIfUnprocessed, index, outputs);
    }
    
    void requestThread(long index, long thread)
    {
        slotAction(&ThreadedPatchSlot::requestThread, index, thread);
    }
    
    void resetProcessed()   { forAllSlots(&ThreadedPatchSlot::resetProcessed); }
    void updateThreads()    { forAllSlots(&ThreadedPatchSlot::updateThread); }
};

#endif /* defined(_PATCHSET_H_) */
