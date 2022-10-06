#pragma once

#include "NonCopy.hpp"
#include "NonMove.hpp"

class Singleton : public NonCopy, public NonMove {};
