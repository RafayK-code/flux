#pragma once

#include <flux/core/delegate/DelegateBase.h>

#include <utility>

/**
* Technically this does the same thing as std::function with less overhead
* 
* We could probabably still use std::function tbh...
*/

namespace flux
{
    template <typename T>
    class Delegate;

    template <typename T>
    class MulticastDelegate;

    template <typename TRet, typename... TParams>
    class Delegate<TRet(TParams...)> : private DelegateBase<TRet(TParams...)>
    {
    public:
        Delegate() = default;

        template <typename TLambda>
        Delegate(const TLambda& lambda)
        {
            AssignInvocation((void*)(&lambda), LambdaStub<TLambda>);
        }

        template <typename TClass, TRet(TClass::*TMethod)(TParams...)>
        static inline Delegate Create(TClass* instance)
        {
            return Delegate(instance, MethodStub<TClass, TMethod>);
        }

        template <typename TClass, TRet(TClass::*TMethod)(TParams...) const>
        static inline Delegate Create(const TClass* instance)
        {
            return Delegate(const_cast<TClass*>(instance), ConstMethodStub<TClass, TMethod>);
        }

        template <typename TLambda>
        static inline Delegate Create(const TLambda& instance)
        {
            return Delegate((void*)(&instance), LambdaStub<TLambda>);
        }

        inline TRet operator()(TParams... params) const
        {
            return (*invocation_.stub)(invocation_.object, std::forward<TParams>(params)...);
        }

        inline bool Null() const { return invocation_.stub == nullptr; }
        explicit operator bool() const { return invocation_.stub != nullptr; }

    private:
        Delegate(void* object, typename DelegateBase<TRet(TParams...)>::StubType stub);
        void AssignInvocation(void* object, typename DelegateBase<TRet(TParams...)>::StubType stub);

        template <typename TClass, TRet(TClass::*TMethod)(TParams...)>
        static inline TRet MethodStub(void* ptr, TParams&&... params)
        {
            TClass* p = static_cast<TClass*>(ptr);
            return (p->*TMethod)(std::forward<TParams>(params)...);
        }

        template <typename TClass, TRet(TClass::*TMethod)(TParams...) const>
        static inline TRet ConstMethodStub(void* ptr, TParams&&... params)
        {
            const TClass* p = static_cast<const TClass*>(ptr);
            return (p->*TMethod)(std::forward<TParams>(params)...);
        }

        template <typename TLambda>
        static inline TRet LambdaStub(void* ptr, TParams&&... params)
        {
            TLambda* p = static_cast<TLambda*>(ptr);
            return (*p)(std::forward<TParams>(params)...);
        }

    private:
        typename DelegateBase<TRet(TParams...)>::InvocationElement invocation_;

        friend class MulticastDelegate<TRet(TParams...)>;
    };

    template <typename TRet, typename... TParams>
    Delegate<TRet(TParams...)>::Delegate(void* object, typename DelegateBase<TRet(TParams...)>::StubType stub)
    {
        invocation_.object = object;
        invocation_.stub = stub;
    }

    template <typename TRet, typename... TParams>
    void Delegate<TRet(TParams...)>::AssignInvocation(void* object, typename DelegateBase<TRet(TParams...)>::StubType stub)
    {
        invocation_.object = object;
        invocation_.stub = stub;
    }
}