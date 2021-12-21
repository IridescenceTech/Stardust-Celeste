/**
 * @file Event.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Event systems
 * @version 0.1
 * @date 2021-12-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once
#include <string>
#include "Dispatcher.hpp"

namespace Stardust_Celeste::Event {

    template <typename T>
    class Event {
    protected:
        T type;
        std::string name;
        bool handled = false;
    public:
        Event() = default;
        
        Event(T t, const std::string& n = "") : type(t), name(n) {};

        inline const T type() const {
            return type;
        }

        inline const std::string& getName() const {
            return name;
        }

        virtual bool isHandled() {
            return handled;
        }
    private:
    };
}