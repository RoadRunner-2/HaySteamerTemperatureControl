/*
  Drawer.h - A simple Arduino library defining a simple interface for a display.
  Created by Eva Wiedner, 2025.
  Released under the MIT License.
*/

#ifndef ACTOR_H
#define ACTOR_H

#include <Arduino.h>

template<typename T>
class Actor {
public:
    virtual ~Actor() {}
    virtual void write(T input_value) = 0;
	virtual void setup() = 0;
};

#endif