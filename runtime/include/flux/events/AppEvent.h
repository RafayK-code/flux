#pragma once

#include <flux/events/Event.h>

namespace flux
{
    class AppUpdateEvent : public EventT<AppUpdateEvent>
    {
    public:
        AppUpdateEvent(float dt) : dt_(dt) {}
        ~AppUpdateEvent() = default;

        inline float GetDt() const { return dt_; }

    private:
        float dt_;
    };

    class AppRenderEvent : public EventT<AppRenderEvent>
    {
    public:
        AppRenderEvent() = default;
        ~AppRenderEvent() = default;
    };
}