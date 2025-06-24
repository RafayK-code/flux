#pragma once

#include <flux/events/Event.h>
#include <flux/core/codes/MouseCodes.h>

namespace flux
{
    class MouseButtonEvent
    {
    public:
        inline MouseButton GetMouseButton() const { return button_; }

        inline float GetX() const { return mouseX_; }
        inline float GetY() const { return mouseY_; }

    protected:
        MouseButtonEvent(MouseButton button, float x, float y) : button_(button), mouseX_(x), mouseY_(y) {}
        ~MouseButtonEvent() = default;

        MouseButton button_;
        float mouseX_;
        float mouseY_;
    };

    class MouseButtonDownEvent : public MouseButtonEvent, public EventT<MouseButtonDownEvent>
    {
    public:
        MouseButtonDownEvent(MouseButton button, float x, float y) : MouseButtonEvent(button, x, y) {}
        ~MouseButtonDownEvent() = default;
    };

    class MouseButtonUpEvent : public MouseButtonEvent, public EventT<MouseButtonUpEvent>
    {
    public:
        MouseButtonUpEvent(MouseButton button, float x, float y) : MouseButtonEvent(button, x, y) {}
        ~MouseButtonUpEvent() = default;
    };

    class MouseMovedEvent : public EventT<MouseMovedEvent>
    {
    public:
        MouseMovedEvent(float x, float y) : mouseX_(x), mouseY_(y) {}
        ~MouseMovedEvent() = default;

        inline float GetX() const { return mouseX_; }
        inline float GetY() const { return mouseY_; }

    private:
        float mouseX_;
        float mouseY_;
    };

    class MouseScrolledEvent : public EventT<MouseScrolledEvent>
    {
    public:
        MouseScrolledEvent(float xOffset, float yOffset) : xOffset_(xOffset), yOffset_(yOffset) {}
        ~MouseScrolledEvent() = default;

        inline float GetXOffset() const { return xOffset_; }
        inline float GetYOffset() const { return yOffset_; }

    private:
        float xOffset_;
        float yOffset_;
    };
}