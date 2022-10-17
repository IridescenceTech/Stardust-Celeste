#pragma once

#include "NonCopy.hpp"
#include "NonMove.hpp"

/**
 * @brief Singletons, which are not moveable or copyable
 *
 */
class Singleton : public NonCopy, public NonMove {};
