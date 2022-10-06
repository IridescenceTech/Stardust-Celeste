#pragma once

namespace Stardust_Celeste::Core {
class Application;

class ApplicationState {
  public:
    ApplicationState() = default;

    ~ApplicationState() = default;

    virtual void on_update(Application *app, double dt) = 0;
    virtual void on_draw(Application *app, double dt) = 0;

    virtual void on_start() = 0;
    virtual void on_cleanup() = 0;
};
} // namespace Stardust_Celeste::Core
