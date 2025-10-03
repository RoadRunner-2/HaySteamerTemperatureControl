#pragma once

template<typename T>
class Sensor {
public:
    virtual ~Sensor() {}
    virtual T read() = 0;
};