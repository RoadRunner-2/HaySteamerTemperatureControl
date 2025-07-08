#pragma once

#include <chrono>
#include <ctime>

// Global static function to get year from Unix timestamp
static int year(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_year + 1900; // tm_year is years since 1900
}

// Global static function to get month from Unix timestamp (1-12)
static int month(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_mon + 1; // tm_mon is 0-11, convert to 1-12
}

// Global static function to get day of month from Unix timestamp (1-31)
static int day(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_mday; // tm_mday is already 1-31
}

// Global static function to get hour from Unix timestamp (0-23)
static int hour(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_hour; // tm_hour is already 0-23
}

// Global static function to get minutes from Unix timestamp (0-59)
static int minute(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_min; // tm_min is already 0-59
}

// Bonus: Global static function to get seconds from Unix timestamp (0-59)
static int second(time_t unixTimeStamp) {
    std::tm* time = std::gmtime(&unixTimeStamp);
    return time->tm_sec; // tm_sec is already 0-59
}
