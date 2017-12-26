
#ifndef __PATCHSET__
#define __PATCHSET__

#include <ext.h>
#include <jpatcher_api.h>
#include <ext_dictionary.h>
#include <ext_dictobj.h>

#include <vector>

#include <AH_Atomic.h>


// Deferred helpers

template <class slotClass> void deleteSlot(t_object *x, t_symbol *s, long argc, t_atom *argv)
{
    delete (slotClass *)atom_getobj(argv);
}

template <class slotClass> void doOpen(t_object *x, t_symbol *s, long argc, t_atom *argv)
{
    ((slotClass *)atom_getobj(argv))->openWindow();
}

template <class slotClass> void doClose(t_object *x, t_symbol *s, long argc, t_atom *argv)
{
    ((slotClass *)atom_getobj(argv))->closeWindow();
}

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

template <class slotClass> class PatchSet
{

    // FIX - safety everywhere...!

public:
    
    PatchSet(t_object *x, t_patcher *parent, long numIns, long numOuts, void **outs) : mOwner(x), mParent(parent), mTargetIndex(0), mIsLoading(0), mNumIns(numIns)
    {
        mOutTable.assign(outs, outs + numOuts);
    }
    
    ~PatchSet()
    {
        for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
            delete *it;
        
        mSlots.clear();
    }
    
    // Size
    
    long size() { return mSlots.size(); }
    
    // Load and Remove
    
    long load(long index, t_symbol *patchName, long argc, t_atom *argv, long vecSize, long samplingRate)
    {
        // Check that this object is not loading in another thread
        
        if (ATOMIC_INCREMENT_BARRIER(&mIsLoading) > 1)
        {
            object_error((t_object *) mOwner, "patch is loading in another thread");
            ATOMIC_DECREMENT_BARRIER(&mIsLoading);
            return -1;
        }
        
        // Find a free patch if no index is given
        
        if (index < 0)
        {
            for (index = 0; index < size(); index++)
                if (mSlots[index]->isEmpty())
                    break;
        }

        // FIX - some kind of high value check?
        
        // Resize if necessary
        
        for (long i = size(); i < index+ 1; i++)
            mSlots.push_back(new slotClass(mOwner, mParent, mNumIns, &mOutTable));
        
        // FIX - do errors!
        //PatchSlot::LoadError error =
        mSlots[index]->load(index + 1, patchName, argc, argv, vecSize, samplingRate);
    
        ATOMIC_DECREMENT_BARRIER(&mIsLoading);
        
        return index;
    }

    void remove(t_atom_long index)
    {
        if (userSlotExists(index))
        {
            mSlots[index - 1]->setInvalid();
            slotClass *toDelete = mSlots[index - 1];
            mSlots[index - 1] = new slotClass(mOwner, mParent, mNumIns, &mOutTable);
                   
            // FIX - errors...
            
            t_atom a;
            atom_setobj(&a, toDelete);
            
            defer(mOwner,(method)deleteSlot<slotClass>, 0L, 1, &a);
        }
    }
    
    void clear()
    {
        for (long i = 1; i <= size(); i++)
            remove(i);
    }
    
    // Update
    
    void update(t_patcher *p, long vecSize, long samplingRate)
    {
        // Reload the patcher when it's updated
    
        for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
        {
            // FIX - this is not complete as a reloader...
            
            if ((*it)->getPatch() == p)
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
        
        // if ((t_ptr_uint) arg < 1 || NOGOOD(arg) || ob_sym(arg) == gensym("dspchain"))
        // return NULL;

        /*
        if (userSlotExists(index + 1))
        {
            if (userIndex)
                *userIndex = mSlots[index]->getUserIndex();
            return mSlots[index]->getPatch();
        }
        */
        
        // Loop over hosted patches to find the next one
        
        long count = 0;
        
        for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
        {
            if ((*it)->getPatch())
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
    
    void objBang()
    {
        target(gensym("bang"), 0, 0);
    }
    
    void objInt(t_atom_long n)
    {
        t_atom n_atom;
        atom_setlong(&n_atom, n);
        target(gensym("int"), 1, &n_atom);
    }
    
    void objFloat(double f)
    {
        t_atom f_atom;
        atom_setfloat(&f_atom, f);
        target(gensym("float"), 1, &f_atom);
    }
    
    void objAnything(t_symbol *s, long argc, t_atom *argv)
    {
        target(s, argc, argv);
    }
    
    // Target Methods
    
    
    void objTarget(long argc, t_atom *argv)
    {
        t_atom_long targetIndex = argc ? atom_getlong(argv) : 0;
        
        if (targetIndex >= 0 || targetIndex <= mSlots.size())
            mTargetIndex = targetIndex;
        else
            mTargetIndex = -1;
    }
    
    void objTargetFree(long argc, t_atom *argv)
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
            if (mSlots[i]->getValid() && !mSlots[i]->getBusy())
            {
                mTargetIndex = i + 1;
                return;
            }
        }
        
        mTargetIndex = -1;
    }
    
    // Process and DSP
    
    bool process(long index, void *tempMem, void **outputs, t_ptr_uint tempMemSize)
    {
        if (!userSlotExists(index + 1))
            return false;
        
        return mSlots[index]->process(tempMem, outputs, tempMemSize);
    }
    
    void compileDSP(long vecSize, long samplingRate)
    {
        for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
            (*it)->compileDSP(vecSize, samplingRate);
    }

    // Window Management
    
    bool openWindow(t_atom_long index)
    {
        if (userSlotExists(index))
        {
            t_atom a;
            atom_setobj(&a, mSlots[index - 1]);
            defer(mOwner,(method)doOpen<slotClass>, 0L, 1, &a);
            
            return true;
        }
        
        return false;
    }
    
    bool closeWindow(t_atom_long index)
    {
        if (!index)
        {
            for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
            {
                t_atom a;
                atom_setobj(&a, *it);
                defer(mOwner,(method)doClose<slotClass>, 0L, 1, &a);
            }
            
            return true;
        }
        
        if (userSlotExists(index))
        {
            t_atom a;
            atom_setobj(&a, mSlots[index - 1]);
            defer(mOwner,(method)doClose<slotClass>, 0L, 1, &a);
            
            return true;
        }
        
        return false;
    }
    
    // Queries
    
    void *getOutputHandle(t_ptr_int index)
    {
        if (userSlotExists(index))
            return mSlots[index - 1]->getOutputsHandle();
        else
            return NULL;
    }
    
    void *getTempMemHandle(t_ptr_int index)
    {
        if (userSlotExists(index))
            return mSlots[index - 1]->getTempMemHandle();
        else
            return NULL;
    }
    
    bool getOn(t_ptr_int index)
    {
        if (userSlotExists(index))
            return mSlots[index - 1]->getOn();
        
        return false;
    }
    
    bool getBusy(t_ptr_int index)
    {
        if (userSlotExists(index))
            return mSlots[index - 1]->getBusy();
        
        return false;
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
    
    void setTempMemSize(t_ptr_int index, t_ptr_uint size)
    {
        if (userSlotExists(index))
            mSlots[index - 1]->setTempMemSize(size);
    }
    
    // Patch serialisation
    
    t_max_err serialise(long *argc, t_atom **argv)
    {
        t_dictionary *dict = dictionary_new();
        //t_dictionary *slotDict = dictionary_new();
        dictionary_appendlong(dict, gensym("slotcount"), size());
          /*
        for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
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
            for (typename std::vector<slotClass *>::iterator it = mSlots.begin(); it != mSlots.end(); it++)
                (*it)->message(inlet, msg, argc, argv);
        }
    }

    bool userSlotExists(t_atom_long slotIndex)
    {
        return slotIndex >= 1 && slotIndex <= mSlots.size();
    }
    
    // Data
   
    t_object *mOwner;
    t_patcher *mParent;
    long mTargetIndex;
    t_int32_atomic mIsLoading;
    
    long mNumIns;
    
    std::vector<void *> mOutTable;
    std::vector<slotClass *> mSlots;
};

#endif /* defined(__PATCHSET__) */
