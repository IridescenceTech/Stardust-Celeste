#pragma once
#include <glm.hpp>

namespace Stardust_Celeste::Audio
{
    class Listener
    {
    public:
        Listener();
        ~Listener();

        auto update(glm::vec3 pos, glm::vec3 vel, glm::vec3 look, glm::vec3 up) -> void;

    private:
    };
} // namespace Stardust_Celest::Audio
