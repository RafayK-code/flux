#pragma once

#include <flux/core/delegate/Delegate.h>

#include <vector>
#include <functional>
#include <algorithm>

namespace flux
{
    template <typename TRet, typename... TParams>
    class MulticastDelegate<TRet(TParams...)> : private DelegateBase<TRet(TParams...)>
    {
    public:
        MulticastDelegate() = default;
        ~MulticastDelegate() = default;

        inline bool Null() const { return invocationList_.empty(); }
        inline size_t Size() const { return invocationList_.size(); }

        MulticastDelegate(const MulticastDelegate&) = delete;
        MulticastDelegate& operator=(const MulticastDelegate&) = delete;

        MulticastDelegate& operator+=(const Delegate<TRet(TParams...)>& delegate)
        {
            if (!delegate)
                return *this;

            invocationList_.push_back(typename DelegateBase<TRet(TParams...)>::InvocationElement(delegate.invocation_.object, delegate.invocation_.stub));
            return *this;
        }

        template <typename TLambda>
        MulticastDelegate& operator+=(const TLambda& lambda)
        {
            Delegate<TRet(TParams...)> d = Delegate<TRet(TParams...)>::template Create<TLambda>(lambda);
            return *this += d;
        }

        MulticastDelegate& operator+=(const MulticastDelegate& other)
        {
            for (auto& item : other.invocationList_)
                invocationList_.push_back(typename DelegateBase<TRet(TParams...)>::InvocationElement(item.object, item.stub));

            return *this;
        }

        MulticastDelegate& operator-=(const Delegate<TRet(TParams...)>& delegate)
        {
            if (!delegate)
                return *this;

            invocationList_.erase(std::remove_if(invocationList_.begin(), invocationList_.end(), [&](const auto& item)
            {
                return item == delegate.invocation_;
            }));

            return *this;
        }

        template <typename TLambda>
        MulticastDelegate& operator-=(const TLambda& lambda)
        {
            Delegate<TRet(TParams...)> d = Delegate<TRet(TParams...)>::template Create<TLambda>(lambda);
            return *this -= d;
        }

        MulticastDelegate& operator-=(const MulticastDelegate& other)
        {
            invocationList_.erase(std::remove_if(invocationList_.begin(), invocationList_.end(), [&](const auto& itemA)
            {
                return std::any_of(other.invocationList_.begin(), other.invocationList_.end(), [&](const auto& itemB)
                {
                    return itemA == itemB;
                });
            }));

            return *this;
        }

        void operator()(TParams&&... params) const
        {
            for (auto& item : invocationList_)
                (*(item.stub))(item.object, std::forward<TParams>(params)...);
        }

        template<typename THandler>
        void operator()(TParams&&... params, THandler handler) const
        {
            size_t index = 0;
            for (auto& item : invocationList_)
            {
                TRet value = (*(item.stub))(item.object, std::forward<TParams>(params)...);
                handler(index, &value);
                ++index;
            }
        }

        void operator()(TParams&&... params, Delegate<void(size_t, TRet*)> handler) const
        {
            operator()<decltype(handler)>(std::forward<TParams>(params)..., handler);
        }

        void operator()(TParams&&... params, std::function<void(size_t, TRet*)> handler) const
        {
            operator()<decltype(handler)>(std::forward<TParams>(params)..., handler);
        }

    private:
        using InvocationList = std::vector<typename DelegateBase<TRet(TParams...)>::InvocationElement>;

        InvocationList invocationList_;
    };
}