#pragma once

namespace Stardust_Celeste::Core {
class Application;

/**
 * @brief Application State Base Class
 * 
 */
class ApplicationState {
  public:
    ApplicationState() = default;
    ~ApplicationState() = default;

    /**
     * @brief Pure Virtual Update
     * 
     * @param app Application instance
     * @param dt Delta Time (Frame Time)
     */
    virtual void on_update(Application *app, double dt) = 0;
    
    /**
     * @brief Pure Virtual Draw
     * 
     * @param app Application instance
     * @param dt Delta Time (Frame Time)
     */
    virtual void on_draw(Application *app, double dt) = 0;

    /**
     * @brief On Start Handler (initializes your state)
     * 
     */
    virtual void on_start() = 0;

    /**
     * @brief On Cleanup Handler (deinitializes your state)
     * 
     */
    virtual void on_cleanup() = 0;
};
} // namespace Stardust_Celeste::Core
