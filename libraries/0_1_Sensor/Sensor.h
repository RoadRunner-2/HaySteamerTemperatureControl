/*
  Sensor.h - A simple Arduino library defining a template interface for sensors.
  Created by Eva Wiedner, 2025.
  Released under the MIT License.
*/

#ifndef SENSOR_H
#define SENSOR_H

template<typename T>
class Sensor {
public:
    virtual ~Sensor() {}
    virtual T read() = 0;
};

#endif