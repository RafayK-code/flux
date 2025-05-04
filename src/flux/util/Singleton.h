#pragma once

#include <utility>

#ifdef FLUX_THREADSAFE
#include <mutex>
#endif

namespace flux
{
    /**
    * Manages singleton resources for a desired singleton class
    */
    template <typename T>
    class Singleton
    {
    public:
        virtual ~Singleton();

        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;

        template <typename... Args>
        static void Init(Args&&... args)
        {
#ifdef FLUX_THREADSAFE
            std::lock_guard<std::mutex> lock(GetMutex());
#endif
            if (!GetHiddenPtr())
                GetHiddenPtr() = new T(std::forward<Args>(args)...);
        }

        static void Shutdown();

        static inline T& Ref() { return *GetHiddenPtr(); }
        static inline T* Ptr() { return GetHiddenPtr(); }

    protected:
        Singleton();

        static inline T*& GetHiddenPtr()
        {
            static T* instance = nullptr;
            return instance;
        }

#ifdef FLUX_THREADSAFE
        static inline std::mutex& GetMutex()
        {
            static std::mutex mtx;
            return mtx;
        }
#endif
    };

    template <typename T>
    Singleton<T>::Singleton()
    {
        GetHiddenPtr() = nullptr;
    }

    template <typename T>
    Singleton<T>::~Singleton()
    {
    }

    template <typename T>
    void Singleton<T>::Shutdown()
    {
#ifdef FLUX_THREADSAFE
        std::lock_guard<std::mutex> lock(GetMutex());
#endif
        delete GetHiddenPtr();
        GetHiddenPtr() = nullptr;
    }
}