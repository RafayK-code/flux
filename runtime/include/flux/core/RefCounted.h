#pragma once

#include <flux/core/base.h>

#ifdef FLUX_THREADSAFE
#include <atomic>
#endif

namespace flux
{
#ifndef FLUX_USE_INTRUSIVE_REF

    class FLUX_API RefCounted {};

#else

    /**
    * Base class for any objects that want to be ref counted
    * 
    * Basic version adapted from godot
    */
    class FLUX_API RefCounted
    {
    public:
        RefCounted();
        virtual ~RefCounted();

        RefCounted(const RefCounted& other);
        RefCounted& operator=(const RefCounted& other);

        /**
        * Since they do the same thing... these move operators dont need to exist... but whatever
        */
        RefCounted(RefCounted&& other) noexcept;
        RefCounted& operator=(RefCounted&& other) noexcept;

        uint32_t RefCount() const { return refCount_.load(); }

    private:
        void IncRef();
        void DecRef();

    private:
#ifdef FLUX_THREADSAFE
        mutable std::atomic<uint32_t> refCount_;
#else
        uint32_t refCount_;
#endif

        template <typename T>
        friend class Ref;
    };

#endif
}