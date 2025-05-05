#include <pch.h>

#include <flux/core/RefCounted.h>
#include <flux/core/LogManager.h>

/**
* NOTE ON IMPLEMENTATION:
* 
* I do some shit with atomics here. Idk if I need to do it this way in cxx17
* but it doesnt hurt I guess. Ill do some more research with atomics and see if I can just treat them like
* a basic uint32
*/

namespace flux
{
#ifdef FLUX_USE_INTRUSIVE_REF

    RefCounted::RefCounted()
        : refCount_(0)
    {
    }

    RefCounted::~RefCounted()
    {
#ifdef FLUX_THREADSAFE
        bool cond = refCount_.load(std::memory_order_acquire) == 0;
#else
        bool cond = refCount_ == 0;
#endif
        DBG_ASSERT(cond, "Ref count somehow became incorrect");
    }

    RefCounted::RefCounted(const RefCounted& other)
        : refCount_(0)
    {
    }

    RefCounted& RefCounted::operator=(const RefCounted& other)
    {
        return *this;
    }

    RefCounted::RefCounted(RefCounted&& other) noexcept
        : refCount_(0)
    {
    }

    RefCounted& RefCounted::operator=(RefCounted&& other) noexcept
    {
        return *this;
    }

    void RefCounted::IncRef()
    {
#ifdef FLUX_THREADSAFE
        refCount_.fetch_add(1, std::memory_order_relaxed);
#else
        refCount_++;
#endif
    }

    void RefCounted::DecRef()
    {
#ifdef FLUX_THREADSAFE
        refCount_.fetch_sub(1, std::memory_order_acq_rel);
#else
        refCount_--;
#endif
    }

#endif
}