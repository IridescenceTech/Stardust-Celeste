#pragma once
#include "Dispatcher.hpp"
#include <string>

namespace Stardust_Celeste::Event {

template <typename T> class Event {
  protected:
    T type;
    std::string name;
    bool handled = false;

  public:
    Event() = default;

    Event(T t, const std::string &n = "") : type(t), name(n){};

    inline const T get_type() const { return type; }

    inline const std::string &get_name() const { return name; }

    virtual bool is_handled() { return handled; }

  private:
};
} // namespace Stardust_Celeste::Event