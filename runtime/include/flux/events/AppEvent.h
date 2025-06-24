#pragma once

#include <flux/events/Event.h>

namespace flux
{
    class AppTickEvent : public EventT<AppTickEvent>
    {
    public:
        AppTickEvent(float dt) : dt_(dt) {}
        ~AppTickEvent() = default;

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