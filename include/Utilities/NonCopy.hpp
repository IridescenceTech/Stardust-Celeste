/**
 * @file NonCopy.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Defines a non-copyable class
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

class NonCopy {
public:
    NonCopy (const NonCopy&) = delete;
    NonCopy& operator = (const NonCopy&) = delete;

protected:
    NonCopy () = default;
    ~NonCopy () = default;
};
