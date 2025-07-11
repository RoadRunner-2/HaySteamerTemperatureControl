#ifndef TIMEREADER_H
#define TIMEREADER_H

#ifdef SANDBOX_ENVIRONMENT
#pragma once

#include <string> // Include the standard string library
using String = std::string; // Define "String" as an alias for std::string
// Define toString(int) as std::to_string
using Fptr = std::string(*)(int);
constexpr Fptr toString = &std::to_string;

#include <chrono>
// Mock implementation of millis() for sandbox environment
inline unsigned long millis() {
    static auto startTime = std::chrono::steady_clock::now();
    auto currentTime = std::chrono::steady_clock::now();
    return static_cast<unsigned long>(std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count());
}
#include "Sandbox/Sensor.h"
#include "Sandbox/Status.h"
#endif

#ifdef ARDUINO
// Define toString(int) as String(int)
using Fptr = std::string(*)(int);
constexpr Fptr toString = String;

// millis() is provided by the Arduino framework, no need to define it
#include <Sensor.h>
#include <Status.h>
#endif

using NTPClock = Sensor<time_t>;

class TimeReader {
public:
    // No interval parameter needed anymore
    TimeReader(NTPClock* clock)
        : clock(clock), lastValue(0) {
    }

    /// This function updates the lastValue with the current ntp time
    /// It should be called periodically to keep the lastValue updated.
    void update() {
        lastValue = clock->read();
    }

    /// Returns the time of day in minutes converted from the last read of the ntp clock
    /// <returns>time of day in minutes</returns>
    int getTimeOfDayInMinutes() const {
        return ((lastValue / 60) % 1440);
    }

    /// Get the current date and time as string ("hh:mm dd.mm.yyyy")
    /// in 24h format
    /// <returns>String representation of the last time and date</returns>
    String getDisplayString() const {
        char buf[18];
		TimeElements tm = breakTime(lastValue);
        snprintf(buf, sizeof(buf), "%02d:%02d %02d.%02d.%04d", 
            tm.Hour, tm.Minute, tm.Day, tm.Month, tm.Year + 1970);
        return String(buf);
    }

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

    TimeElements breakTime(time_t timeInput) const {
        // break the given time_t into time components
        // this is a more compact version of the C library localtime function
        // note that year is offset from 1970
		TimeElements tm;

        uint8_t year;
        uint8_t month, monthLength;
        uint32_t time;
        unsigned long days;

        time = (uint32_t)timeInput;
        tm.Second = time % 60;
        time /= 60; // now it is minutes
        tm.Minute = time % 60;
        time /= 60; // now it is hours
        tm.Hour = time % 24;
        time /= 24; // now it is days
        tm.Wday = ((time + 3) % 7) + 1; // monday is day 1

        year = 0;
        days = 0;
        while ((unsigned)(days += (isLeapYear(year) ? 366 : 365)) <= time) {
            year++;
        }
        tm.Year = year; // year is offset from 1970 

        days -= isLeapYear(year) ? 366 : 365;
        time -= days; // now it is days in this year, starting at 0

        days = 0;
        month = 0;
        monthLength = 0;
        for (month = 0; month < 12; month++) {
            if (month == 1) { // february
                if (isLeapYear(year)) {
                    monthLength = 29;
                }
                else {
                    monthLength = 28;
                }
            }
            else {
                monthLength = monthDays[month];
            }

            if (time >= monthLength) {
                time -= monthLength;
            }
            else {
                break;
            }
        }
        tm.Month = month + 1;  // jan is month 1  
        tm.Day = time + 1;     // day of month

		return tm;
    }
    constexpr static uint8_t monthDays[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

    NTPClock* clock;
	time_t lastValue; // in seconds since epoch (1970-01-01 00:00:00 UTC), converted to local time
};

#endif
