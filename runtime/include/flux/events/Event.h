#pragma once

#include <functional>
#include <vector>
#include <string>
#include <map>
#include <vector>

#include <flux/events/EventTypeRegistry.h>

namespace flux
{
    class FLUX_API AbstractEvent
    {
    public:
        AbstractEvent() = default;
        virtual ~AbstractEvent() = default;

        virtual uint32_t GetThisEventTypeID() const = 0;
    };

    template <typename T>
    class EventT : public AbstractEvent
    {
    public:
        EventT() = default;
        virtual ~EventT() = default;

        virtual uint32_t GetThisEventTypeID() const override { return GetEventTypeID(); }

        static uint32_t GetEventTypeID() { return EventTypeRegistry::GetEventTypeID<T>(); }
    };

    template<typename E>
    using EventCallbackFn = std::function<void(const E&)>;

    class FLUX_API EventListener;

    class FLUX_API EventDispatcher
    {
    public:
        EventDispatcher() = default;
        ~EventDispatcher();

        template <typename E>
        void AddListener(EventListener* listener)
        {
            RemoveListenerForEvent<E>(listener);
            listeners_[E::GetEventTypeID()].push_back(listener);
        }

        template <typename E>
        void RemoveListenerForEvent(EventListener* listener)
        {
            std::vector<EventListener*>& vec = listeners_[E::GetEventTypeID()];
            auto it = std::find(vec.begin(), vec.end(), listener);
            if (it != vec.end())
                vec.erase(it);
        }

        void RemoveListener(EventListener* listener)
        {
            for (auto& pair : listeners_)
            {
                std::vector<EventListener*>& vec = pair.second;
                auto it = std::find(vec.begin(), vec.end(), listener);
                if (it != vec.end())
                    vec.erase(it);
            }
        }

        template<typename E>
        void DispatchEvent(const E& e);

    private:
        using ListenerMap = std::map<uint32_t, std::vector<EventListener*>>;
        ListenerMap listeners_;
    };

    class FLUX_API EventListener
    {
    public:
        EventListener() = default;
        ~EventListener();

        template <typename E>
        void Listen(EventDispatcher& dispatcher, const EventCallbackFn<E>& callback)
        {
            auto wrapper = [callback](const AbstractEvent* event)
                {
                    if (const E* e = dynamic_cast<const E*>(event))
                        callback(*e);
                };

            dispatchers_.insert(&dispatcher);

            dispatcher.AddListener<E>(this);
            callbacks_[&dispatcher][E::GetEventTypeID()] = wrapper;
        }

        template <typename E>
        void StopListeningForEvent()
        {
            for (EventDispatcher* dispatcher : dispatchers_)
            {
                StopListeningForEvent<E>(*dispatcher);
            }
        }

        template <typename E>
        void StopListeningForEvent(EventDispatcher& dispatcher)
        {
            auto it = dispatchers_.find(&dispatcher);
            if (it == dispatchers_.end())
                return;

            dispatcher.RemoveListenerForEvent<E>(this);
            callbacks_[&dispatcher].erase(E::GetEventTypeID());
        }

        void StopListening();
        void StopListening(EventDispatcher& dispatcher);

    private:
        friend class EventDispatcher;

        void HandleEvent(EventDispatcher* dispatcher, const AbstractEvent* event);
        void EventDispatcherDestroyed(EventDispatcher* dispatcher);

    private:
        using BaseEventCallbackFn = std::function<void(const AbstractEvent*)>;
        using EventCallbackMap = std::map<EventDispatcher*, std::map<uint32_t, BaseEventCallbackFn>>;

        EventCallbackMap callbacks_;
        std::set<EventDispatcher*> dispatchers_;
    };

    template<typename E>
    void EventDispatcher::DispatchEvent(const E& e)
    {
        auto it = listeners_.find(E::GetEventTypeID());
        if (it == listeners_.end())
            return;

        for (EventListener* listener : it->second)
            listener->HandleEvent(this, &e);
    }
}