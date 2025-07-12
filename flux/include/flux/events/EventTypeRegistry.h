#pragma once

#include <flux/core/base.h>

#include <typeindex>
#include <unordered_map>
#include <cstdint>

namespace flux
{
    class FLUX_API EventTypeRegistry
    {
    public:
        template<typename T>
        static uint32_t GetEventTypeID()
        {
            static const uint32_t id = RegisterType(typeid(T));
            return id;
        }

    private:
        static uint32_t RegisterType(std::type_index type);
    };
}