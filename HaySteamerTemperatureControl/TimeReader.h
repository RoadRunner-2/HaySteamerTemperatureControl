#ifndef TIMEREADER_H
#define TIMEREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include "Sandbox/StringConversion.h"
#include "Sandbox/millis.h"
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"
#include "Sandbox/CyclicModule.h"
#endif

#ifdef ARDUINO
#include <Sensor.h>
#include <Status.h>
#include <CyclicModule.h>
#include <TimeLib.h>
#include <Arduino.h>
#endif

using NTPClock = Sensor<time_t>;

class TimeReader : public CyclicModule {
public:
    TimeReader(NTPClock* clock);

    /// Updates the lastValue with the current ntp time
    /// It should be called periodically to keep the lastValue updated.
    void update() override;

    /// Returns the time of day in minutes converted from the last read of the ntp clock
    /// <returns>time of day in minutes</returns>
    int getTimeOfDayInMinutes() const;

    /// Get the current date and time as string ("hh:mm dd.mm.yyyy")
    /// in 24h format
    /// <returns>String representation of the last time and date</returns>
    String getDisplayString() const;

private:
    struct TimeElements {
        uint8_t Second; // 0-59
        uint8_t Minute; // 0-59
        uint8_t Hour;   // 0-23
        uint8_t Wday;   // 1-7, monday is day 1
        uint8_t Day;    // 1-31
        uint8_t Month;  // 1-12
        uint8_t Year;   // offset from 1970; 0-255
	} ;

    constexpr static bool isLeapYear(uint8_t yearOffsetFrom1970) {
        int year = 1970 + yearOffsetFrom1970;
        return (year > 0) && !(year % 4) && ((year % 100) || !(year % 400));
    }
    constexpr static uint8_t monthDays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    TimeElements breakTime(time_t timeInput) const;
    
    NTPClock* clock;
	time_t lastValue; // in seconds since epoch (1970-01-01 00:00:00 UTC), converted to local time
};

#endif
