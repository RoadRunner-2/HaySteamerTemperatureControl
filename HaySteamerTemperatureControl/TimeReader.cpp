#include "TimeReader.h"

TimeReader::TimeReader(NTPClock* clock)
    : clock(clock), lastValue(0) {
};

void TimeReader::update() {
    lastValue = clock->read();
};

int TimeReader::getTimeOfDayInMinutes() const {
    return ((lastValue / 60) % 1440);
};

String TimeReader::getDisplayString() const {
    char buf[18];
    TimeElements tm = breakTime(lastValue);
    snprintf(buf, sizeof(buf), "%02d:%02d %02d.%02d.%04d",
        tm.Hour, tm.Minute, tm.Day, tm.Month, tm.Year + 1970);
    return String(buf);
};

TimeReader::TimeElements TimeReader::breakTime(time_t timeInput) const {
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
};