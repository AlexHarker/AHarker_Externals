
#ifndef __PATCHSET__
#define __PATCHSET__

#include <ext.h>
#include <jpatcher_api.h>
#include <ext_dictionary.h>
#include <ext_dictobj.h>

#include <memory>
#include <vector>


// dspchain Attribute Helper

static t_max_err patchset_get_ownsdspchain(t_object *pv, t_object *attr, long *argc, t_atom **argv)
{
    char alloc = false;
    
    if (atom_alloc(argc, argv, &alloc))
        return MAX_ERR_GENERIC;
    
    // This prevents getdspchain on child patchers from walking up past this object
    
    atom_setlong(*argv,1);
    return MAX_ERR_NONE;
}

// Main class

template <class SlotClass>
class PatchSet
{

    // FIX - safety everywhere...!

    static SlotClass *slotFromAtom(t_atom *argv)
    {
        return reinterpret_cast<SlotClass *>(atom_getobj(argv));
    }
    
    void deferSlotAction(SlotClass *slot, method action)
    {
        t_atom a;
        atom_setobj(&a, slot);
        defer(mOwner, action, 0L, 1, &a);
    }
    
    void deferSlotAction(t_atom_long index, method action)
    {
        if (userSlotExists(index))
            deferSlotAction(mSlots[index - 1].get(), action);
    }
    
public:
    
    // Deferred helpers
    
    static void doLoad(t_object *x, t_symbol *s, long argc, t_atom *argv)
    {
        PatchSet *set = (PatchSet *)atom_getobj(argv + 0);
        long index = (long) atom_getlong(argv + 1);
        long vecSize = (long) atom_getlong(argv + 2);
        long samplingRate = (long) atom_getlong(argv + 3);

        set->loadDeferred(index, s, argc - 4, argv + 4, vecSize, samplingRate);
    }
    
    static void deleteSlot(t_object *x, t_symbol *s, long argc, t_atom *argv)
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
    
    PatchSet(t_object *x, t_patcher *parent, long numIns, long numOuts, void **outs) : mOwner(x), mParent(parent), mTargetIndex(0), mNumIns(numIns)
    {
        mOutTable.assign(outs, outs + numOuts);
    }
    
    // Size
    
    long size() { return mSlots.size(); }
    
    // Load and Remove
    
    long load(long index, t_symbol *patchName, long argc, t_atom *argv, long vecSize, long samplingRate)
    {
        std::vector<t_atom> newArgv(argc + 4);

        // FIX - some kind of high value check for user specified index?

        // Find a free patch if no index is given
        
        if (index < 0)
        {
            for (index = 0; index < size(); index++)
                if (!mSlots[index])
                    break;
        }

        atom_setobj(newArgv.data() + 0, this);
        atom_setlong(newArgv.data() + 1, index);
        atom_setlong(newArgv.data() + 2, vecSize);
        atom_setlong(newArgv.data() + 3, samplingRate);
        
        for (long i = 0; i < argc; i++)
            newArgv[i + 4] = argv[i];
        
        defer(mOwner, (method)doLoad, patchName, argc + 4, newArgv.data());
    
        return index;
    }
    
    void loadDeferred(long index, t_symbol *patchName, long argc, t_atom *argv, long vecSize, long samplingRate)
    {
        // Resize if necessary
        
        if (index >= size())
            mSlots.resize(index + 1);
        
        mSlots[index] = std::unique_ptr<SlotClass>(new SlotClass(mOwner, mParent, mNumIns, &mOutTable));
        PatchSlot::LoadError error = mSlots[index]->load(index + 1, patchName, argc, argv, vecSize, samplingRate);
        
        if (error != PatchSlot::LoadError::kNone)
        {
            object_error(mOwner, "error trying to load patch %s - %s", patchName->s_name, PatchSlot::errString(error));
            mSlots[index] = nullptr;
        }
    }

    void remove(t_atom_long index)
    {
        if (userSlotExists(index))
        {
            mSlots[index - 1]->setInvalid();
            deferSlotAction(mSlots[index - 1].release(), (method) deleteSlot);
        }
        else
            object_error(mOwner, "no patch in slot %ld", index);
    }
    
    void clear()
    {
        for (t_atom_long i = 1; i <= size(); i++)
            if (userSlotExists(i))
                remove(i);
    }
    
    // Update
    
    void update(t_patcher *p, long vecSize, long samplingRate)
    {
        // Reload the patcher when it's updated
    
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
        {
            // FIX - this is not complete as a reloader... (keep on state and follow patch)
            
            if ((*it) && (*it)->getPatch() == p)
            {
                (*it)->load(vecSize, samplingRate);
                break;
            }
        }
    }
    
    // Get patch for subpatcher reporting
    
    const t_patcher* reportSubpatch(long index, void *arg, long *userIndex = NULL)
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
        
        return NULL;
    }
    
    // Message Communication
    
    void messageBang()
    {
        target(gensym("bang"), 0, nullptr);
    }
    
    void messageInt(t_atom_long n)
    {
        t_atom n_atom;
        atom_setlong(&n_atom, n);
        target(gensym("int"), 1, &n_atom);
    }
    
    void messageFloat(double f)
    {
        t_atom f_atom;
        atom_setfloat(&f_atom, f);
        target(gensym("float"), 1, &f_atom);
    }
    
    void messageAnything(t_symbol *s, long argc, t_atom *argv)
    {
        target(s, argc, argv);
    }
    
    // Target Methods
    
    void messageTarget(long argc, t_atom *argv)
    {
        t_atom_long targetIndex = argc ? atom_getlong(argv) : 0;
        
        if (targetIndex >= 0 || targetIndex <= mSlots.size())
            mTargetIndex = targetIndex;
        else
            mTargetIndex = -1;
    }
    
    void messageTargetFree(long argc, t_atom *argv)
    {
        long maxSlot = mSlots.size();
        long lo = 0;
        long hi = maxSlot;
        
        t_atom_long in1 = argc > 0 ? atom_getlong(argv + 0) : 0;
        t_atom_long in2 = argc > 1 ? atom_getlong(argv + 1) : 0;
        
        // Clip inputs
        
        in1 = (in1 < 1) ? 1 : ((in1 > maxSlot) ? maxSlot : in1);
        in2 = (in2 < 1) ? 1 : ((in2 > maxSlot) ? maxSlot : in2);
        
        // Load arguments
        
        if (argc)
        {
            if (argc > 1)
            {
                lo = ((in1 < in2) ? in1 : in2) - 1;
                hi = ((in1 > in2) ? in1 : in2);
            }
            else
                hi = in1;
        }
        
        // Search for a free voice
        
        for (long i = lo; i < hi; i++)
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
    
    bool process(long index, void **outputs)
    {
        if (!userSlotExists(index + 1))
            return false;
        
        return mSlots[index]->process(outputs);
    }
    
    void compileDSP(long vecSize, long samplingRate)
    {
        for (auto it = mSlots.begin(); it != mSlots.end(); it++)
            if (*it) (*it)->compileDSP(vecSize, samplingRate);
    }

    // Window Management
    
    void openWindow(t_atom_long index)
    {
        deferSlotAction(index, (method) doOpen);
    }
    
    void closeWindow(t_atom_long index)
    {
        if (!index)
        {
            for (t_atom_long i = 1; i <= size(); i++)
                deferSlotAction(i, (method) doClose);
        }
        else
            deferSlotAction(index, (method) doClose);
    }
    
    // Queries
    
    void *getOutputHandle(t_ptr_int index)
    {
        return userSlotExists(index) ? mSlots[index - 1]->getOutputsHandle() : nullptr;
    }
    
    bool getOn(t_ptr_int index)
    {
        return userSlotExists(index) ? mSlots[index - 1]->getOn() : false;
    }
    
    bool getBusy(t_ptr_int index)
    {
        return userSlotExists(index) ? mSlots[index - 1]->getBusy() : false;
    }
    
    void setOn(t_ptr_int index, t_ptr_int state)
    {
        if (userSlotExists(index))
            mSlots[index - 1]->setOn(state ? true : false);
    }
    
    void setBusy(t_ptr_int index, t_ptr_int state)
    {
        if (userSlotExists(index))
            mSlots[index - 1]->setBusy(state ? true : false);
    }
    
    // Patch serialisation
    
    t_max_err serialise(long *argc, t_atom **argv)
    {
        t_dictionary *dict = dictionary_new();
        //t_dictionary *slotDict = dictionary_new();
        dictionary_appendlong(dict, gensym("slotcount"), size());
          /*
        for (typename std::vector<SlotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
            dictionary_appenddictionary(slotDict, gensym("key"), val)(*it)->serialise();
        */
        
        t_max_err error = dictobj_dictionarytoatoms(dict, argc, argv);
        object_free(dict);
        
        return error;
    }
    
protected:

    void target(t_symbol *msg, long argc, t_atom *argv)
    {
        long inlet = proxy_getinlet(mOwner);
        
        if (mTargetIndex)
        {
            if (userSlotExists(mTargetIndex))
                mSlots[mTargetIndex - 1]->message(inlet, msg, argc, argv);
        }
        else
        {
            for (auto it = mSlots.begin(); it != mSlots.end(); it++)
                if (*it) (*it)->message(inlet, msg, argc, argv);
        }
    }

    bool userSlotExists(t_atom_long slotIndex)
    {
        return slotIndex >= 1 && slotIndex <= mSlots.size() && mSlots[slotIndex - 1];
    }
    
    // Data
   
    t_object *mOwner;
    t_patcher *mParent;
    long mTargetIndex;
    
    long mNumIns;
    
    std::vector<void *> mOutTable;
    std::vector<std::unique_ptr<SlotClass>> mSlots;
};

#endif /* defined(__PATCHSET__) */
