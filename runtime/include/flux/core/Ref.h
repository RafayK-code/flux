#pragma once

#include <flux/core/RefCounted.h>

#include <type_traits>
#include <functional>
#include <memory>

namespace flux
{
    template <typename T>
    using Box = std::unique_ptr<T>;

    template <typename T, typename... Args>
    constexpr Box<T> CreateBox(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

#ifndef FLUX_USE_INTRUSIVE_REF

    template <typename T>
    using Ref = std::shared_ptr<T>;

    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

#else

    template <typename T>
    class Ref
    {
        static_assert(std::is_base_of<RefCounted, T>::value, "T must derive from RefCounted");

    public:
        Ref();
        ~Ref();

        Ref(T* ptr);
        Ref<T>& operator=(T* ptr);

        Ref(const Ref<T>& rptr);
        Ref<T>& operator=(const Ref<T>& rptr);

        Ref(Ref<T>&& rptr) noexcept;
        Ref<T>& operator=(Ref<T>&& other) noexcept;

        T* Get() { return rcPtr_; }
        T* operator->() { return rcPtr_; }

        const T* Get() const { return rcPtr_; }
        const T* operator->() const { return rcPtr_; }

        T& operator*() { return *rcPtr_; }
        const T& operator*() const { return *rcPtr_; }

        bool operator==(const Ref& other) const;
        bool operator==(T* other) const;
        bool operator==(const void* other) const;
        bool operator==(std::nullptr_t) const;

        bool operator!=(const Ref& other) const;
        bool operator!=(T* other) const;
        bool operator!=(const void* other) const;
        bool operator!=(std::nullptr_t) const;

        uint32_t RefCount() const { return rcPtr_->RefCount(); }
        bool Null() const { return rcPtr_ == nullptr; }

        explicit operator bool() const { return rcPtr_ != nullptr; }

        template <typename T2>
        operator Ref<T2>() { return rcPtr_; }

        template <typename T2>
        operator T2* () { return rcPtr_; }

        template <typename T2>
        operator const T2* () const { return rcPtr_; }

        template <typename T2>
        Ref<T2> DynamicCast() const { return (dynamic_cast<T2*>(rcPtr_)); }

    private:
        void IncRef();
        void DecRef();

    private:
        T* rcPtr_;
    };

    template <typename T, typename... Args>
    Ref<T> CreateRef(Args&&... args)
    {
        return Ref<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T>
    Ref<T>::Ref()
        : rcPtr_(nullptr)
    {
    }

    template <typename T>
    Ref<T>::~Ref()
    {
        DecRef();
    }

    template <typename T>
    Ref<T>::Ref(T* ptr)
    {
        rcPtr_ = ptr;
        IncRef();
    }

    template <typename T>
    Ref<T>& Ref<T>::operator=(T* ptr)
    {
        if (rcPtr_ != ptr)
        {
            DecRef();
            rcPtr_ = ptr;
            IncRef();
        }

        return *this;
    }

    template <typename T>
    Ref<T>::Ref(const Ref<T>& rptr)
    {
        rcPtr_ = rptr.rcPtr_;
        IncRef();
    }

    template <typename T>
    Ref<T>& Ref<T>::operator=(const Ref<T>& rptr)
    {
        if (this != &rptr)
        {
            DecRef();
            rcPtr_ = rptr.rcPtr_;
            IncRef();
        }

        return *this;
    }

    template <typename T>
    Ref<T>::Ref(Ref<T>&& rptr) noexcept
    {
        rcPtr_ = rptr.rcPtr_;
        rptr.rcPtr_ = nullptr;
    }

    template <typename T>
    Ref<T>& Ref<T>::operator=(Ref<T>&& rptr) noexcept
    {
        if (this != &rptr)
        {
            DecRef();
            rcPtr_ = rptr.rcPtr_;
            rptr.rcPtr_ = nullptr;
        }

        return *this;
    }

    template <typename T>
    bool Ref<T>::operator==(const Ref& other) const
    {
        return rcPtr_ == other.rcPtr_;
    }

    template <typename T>
    bool Ref<T>::operator==(T* other) const
    {
        return rcPtr_ == other;
    }

    template <typename T>
    bool Ref<T>::operator==(const void* other) const
    {
        return rcPtr_ == other;
    }

    template <typename T>
    bool Ref<T>::operator==(std::nullptr_t) const
    {
        return rcPtr_ == nullptr;
    }

    template <typename T>
    bool Ref<T>::operator!=(const Ref& other) const
    {
        return rcPtr_ != other.rcPtr_;
    }

    template <typename T>
    bool Ref<T>::operator!=(T* other) const
    {
        return rcPtr_ != other;
    }

    template <typename T>
    bool Ref<T>::operator!=(const void* other) const
    {
        return rcPtr_ != other;
    }

    template <typename T>
    bool Ref<T>::operator!=(std::nullptr_t) const
    {
        return rcPtr_ != nullptr;
    }

    template <typename T>
    void Ref<T>::IncRef()
    {
        if (rcPtr_)
            rcPtr_->IncRef();
    }

    template <typename T>
    void Ref<T>::DecRef()
    {
        if (rcPtr_)
        {
            rcPtr_->DecRef();

            if (rcPtr_->RefCount() == 0)
            {
                delete rcPtr_;
                rcPtr_ = nullptr;
            }
        }
    }

#endif
}