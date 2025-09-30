#pragma once

#include <string>
using String = std::string;

template<typename T>
class Actor {
public:
    virtual ~Actor() {}
	virtual void setup() = 0;
    virtual void write(T value) = 0;
};