/**
 * @file Dispatcher.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Dispatches events to an observer
 * @version 0.1
 * @date 2021-12-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <map>
#include <vector>
#include <algorithm>
#include <functional>
#include "Event.hpp"

namespace Stardust_Celeste::Event {

    template <typename T>
    class Event;

    template <typename T>
    class Dispatcher {
    private:
        using SlotType = std::function<void(const Event<T>&)>;
        std::map<T, std::vector<SlotType>> observers;
    public:

        void subscribe(T type, const SlotType& func){
            observers[type].push_back(func);
        }

        void post(Event<T>& event) {
            if(observers.find(event.getType()) == observers.end())
                return;

            for(auto&& observer : observers.at(event.getType()))
                if(!event.isHandled())
                    observer(event);  
        }

    };
}