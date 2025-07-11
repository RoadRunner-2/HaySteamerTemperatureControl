#pragma once

#include <string>
using String = std::string;

template<int arrayLength>
class Drawer {
public:
    virtual ~Drawer() {}
    virtual void write(const String (&content)[arrayLength]) = 0;
};