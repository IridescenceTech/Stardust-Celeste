/**
 * @file NonMove.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Defines a non-movable class
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

class NonMove {
public:
    NonMove (const NonMove&&) = delete;
    NonMove& operator = (const NonMove&&) = delete;

protected:
    NonMove () = default;
    ~NonMove () = default;
};
