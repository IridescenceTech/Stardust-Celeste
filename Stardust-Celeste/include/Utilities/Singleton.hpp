/**
 * @file Singleton.hpp
 * @author Nathan Bourgeois <iridescentrosesfall@gmail.com>
 * @brief Defines a singleton class
 * @version 0.1
 * @date 2021-12-11
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#pragma once

#include "NonCopy.hpp"
#include "NonMove.hpp"

class Singleton : public NonCopy, public NonMove {};