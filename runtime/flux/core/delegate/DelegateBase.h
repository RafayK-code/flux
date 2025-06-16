#pragma once

namespace flux
{
    template <typename T>
    class DelegateBase;

    template <typename TRet, typename... TParams>
    class DelegateBase<TRet(TParams...)>
    {
    protected:
        using StubType = TRet(*)(void* objptr, TParams&&...);

        class InvocationElement
        {
        public:
            InvocationElement() = default;
            InvocationElement(void* objptr, StubType stub)
                : object(objptr), stub(stub)
            {}

            bool operator==(const InvocationElement& other) const
            {
                return object == other.object && stub == other.stub;
            }

            bool operator!=(const InvocationElement& other) const
            {
                return object != other.object || stub != other.stub;
            }

        public:
            void* object = nullptr;
            StubType stub = nullptr;
        };
    };
}