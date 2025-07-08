#pragma once

class Sensor {
public:
    virtual ~Sensor() {}
    virtual double read() = 0;
    virtual void initialize() = 0;
};