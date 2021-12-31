#pragma once

namespace Stardust_Celeste::Utilities::Input {

    enum class Action {
        Forward     = 0,
        Backward    = 1,
        Left        = 2,
        Right       = 3,
        Primary     = 4,
        Alternate   = 5,
    };

    auto update_input() -> void;
    auto get_action_state(Action) -> bool;
    auto get_cursor_pos(float& x, float& y) -> void;
}