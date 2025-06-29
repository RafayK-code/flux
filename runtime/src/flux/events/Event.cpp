#include <fxpch.h>

#include <flux/events/Event.h>

namespace flux
{
    EventDispatcher::~EventDispatcher()
    {
        for (auto& pair : listeners_)
        {
            for (EventListener* listener : pair.second)
                listener->EventDispatcherDestroyed(this);
        }
    }

    EventListener::~EventListener()
    {
        StopListening();
    }

    void EventListener::StopListening()
    {
        for (EventDispatcher* dispatcher : dispatchers_)
        {
            StopListening(*dispatcher);
        }
    }

    void EventListener::StopListening(EventDispatcher& dispatcher)
    {
        auto it = dispatchers_.find(&dispatcher);
        if (it == dispatchers_.end())
            return;

        dispatcher.RemoveListener(this);
        callbacks_[&dispatcher].clear();
    }

    void EventListener::HandleEvent(EventDispatcher* dispatcher, const AbstractEvent* event)
    {
        auto it = callbacks_[dispatcher].find(event->GetThisEventTypeID());
        if (it != callbacks_[dispatcher].end())
            it->second(event);
    }

    void EventListener::EventDispatcherDestroyed(EventDispatcher* dispatcher)
    {
        callbacks_.erase(dispatcher);
        dispatchers_.erase(dispatcher);
    }
}