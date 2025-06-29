#pragma once

#include <flux/events/Event.h>
#include <flux/core/codes/KeyCodes.h>

namespace flux
{
    class KeyEvent
    {
    public:
        inline KeyCode GetKeyCode() const { return keycode_; }

    protected:
        KeyEvent(KeyCode keycode) : keycode_(keycode) {}
        ~KeyEvent() = default;

        KeyCode keycode_;
    };

    class KeyDownEvent : public KeyEvent, public EventT<KeyDownEvent>
    {
    public:
        KeyDownEvent(KeyCode keycode) : KeyEvent(keycode) {}
        ~KeyDownEvent() = default;
    };

    class KeyHeldEvent : public KeyEvent, public EventT<KeyHeldEvent>
    {
    public:
        KeyHeldEvent(KeyCode keycode) : KeyEvent(keycode) {}
        ~KeyHeldEvent() = default;
    };

    class KeyUpEvent : public KeyEvent, public EventT<KeyUpEvent>
    {
    public:
        KeyUpEvent(KeyCode keycode) : KeyEvent(keycode) {}
        ~KeyUpEvent() = default;
    };

    class KeyTypedEvent : public EventT<KeyTypedEvent>
    {
    public:
        KeyTypedEvent(const std::string& text) : text_(text) {}
        ~KeyTypedEvent() = default;

        inline const std::string& Text() const { return text_; }

    private:
        std::string text_;
    };
}