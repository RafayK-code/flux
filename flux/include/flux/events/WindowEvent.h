#pragma once

#include <flux/events/Event.h>

namespace flux
{
    class WindowCloseEvent : public EventT<WindowCloseEvent>
    {
    public:
        WindowCloseEvent() = default;
        ~WindowCloseEvent() = default;
    };

    class WindowResizeEvent : public EventT<WindowResizeEvent> 
    {
    public:
        WindowResizeEvent(int width, int height) : width_(width), height_(height) {}
        ~WindowResizeEvent() = default;

        inline uint32_t Width() const { return width_; }
        inline uint32_t Height() const { return height_; }

    private:
        uint32_t width_;
        uint32_t height_;
    };

    class WindowFocusEvent : public EventT<WindowFocusEvent>
    {
    public:
        WindowFocusEvent() = default;
        ~WindowFocusEvent() = default;

    };

    class WindowLostFocusEvent : public EventT<WindowLostFocusEvent>
    {
    public:
        WindowLostFocusEvent() = default;
        ~WindowLostFocusEvent() = default;
    };

    class WindowMovedEvent : public EventT<WindowMovedEvent>
    {
    public:
        WindowMovedEvent(int xpos, int ypos) : xpos_(xpos), ypos_(ypos) {}
        ~WindowMovedEvent() = default;

        inline int GetXPos() const { return xpos_; }
        inline int GetYPos() const { return ypos_; }

    private:
        int xpos_;
        int ypos_;
    };
}