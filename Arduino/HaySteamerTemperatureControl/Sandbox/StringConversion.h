#pragma once

#include <string>

using String = std::string;

using Fptr = std::string(*)(int);
constexpr Fptr toString = &std::to_string;