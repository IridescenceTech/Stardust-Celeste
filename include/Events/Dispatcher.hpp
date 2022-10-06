#include "Event.hpp"
#include <algorithm>
#include <functional>
#include <map>
#include <vector>

namespace Stardust_Celeste::Event {

template <typename T> class Event;

template <typename T> class Dispatcher {
  private:
    using SlotType = std::function<void(const Event<T> &)>;
    std::map<T, std::vector<SlotType>> observers;

  public:
    void subscribe(T type, const SlotType &func) {
        observers[type].push_back(func);
    }

    void post(Event<T> &event) {
        if (observers.find(event.get_type()) == observers.end())
            return;

        for (auto &&observer : observers.at(event.get_type()))
            if (!event.is_handled())
                observer(event);
    }
};
} // namespace Stardust_Celeste::Event