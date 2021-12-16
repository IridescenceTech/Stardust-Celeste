/**
 * @file ThreadSafe.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief A ThreadSafe class for easy threadsafing
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#pragma once
#include <thread>
#include <mutex>

#ifdef PSP
namespace std {
    class mutex {
        public:

            mutex() : is_locked(false) {}
            ~mutex() = default;

            mutex (const mutex&) = delete;
            mutex& operator = (const mutex&) = delete;

            void lock() {
                while(is_locked) {
                    //Wait for unlock
                }

                is_locked = true;
            }

            void try_lock() {
                if(!is_locked)
                    is_locked = true;
            }

            void unlock() {
                is_locked = false;
            }

        private:
            volatile bool is_locked;
    };
}
#endif

template <typename T>
class ThreadSafe final {
    T data;
    std::mutex mut;

    class Proxy {
        std::unique_lock<std::mutex> lock;
        T& t;
        
        public:

        explicit Proxy(std::mutex& mx, T& t) : lock(mx), t(t) { }
        Proxy(Proxy&& src) : lock(std::move(src.lock)), t(src.t) { }

        ~Proxy() { }
        T* operator -> () { return &t; }
    };


    public:
    
    template <typename ...Args>
    explicit ThreadSafe(Args&&... args) : data(std::forward<Args>(args)...) { }

    Proxy operator -> () { return Proxy(mut, data); }

    template <typename Func, typename R = typename std::enable_if<!std::is_void<typename std::result_of<Func(T&)>::type>::value>::type>
    auto operator () (const Func& func) {
        std::lock_guard<std::mutex> lock(mut);
        return func(data);
    }

    template <typename Func, typename R = typename std::enable_if<std::is_void<typename std::result_of<Func(T&)>::type>::value>::type>
    void operator () (const Func& func) {
        func(*operator -> () . operator -> ());
    }
};