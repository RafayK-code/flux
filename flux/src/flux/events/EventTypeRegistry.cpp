#include <fxpch.h>

#include <flux/events/EventTypeRegistry.h>

namespace flux
{
    static std::unordered_map<std::type_index, uint32_t> gEventTypeMap;
    static uint32_t gEventIDCounter = 0;

    uint32_t EventTypeRegistry::RegisterType(std::type_index type)
    {
        auto it = gEventTypeMap.find(type);
        if (it != gEventTypeMap.end())
            return it->second;

        uint32_t id = gEventIDCounter++;
        gEventTypeMap[type] = id;
        return id;
    }
}