#include "gtest/gtest.h"
#include "../TimeLib.h"
#include <chrono>

// Helper: Unix timestamps for known dates using chrono (UTC)
const int UNIX_1970_01_01_00_00_00 = 0;
const int UNIX_2000_02_29_12_34_56 = 951827696;
const int UNIX_2020_12_31_23_59_59 = 1609459199;
const int UNIX_2021_01_01_00_00_00 = 1609459200;
const int UNIX_2016_02_29_00_00_00 = 1456704000;
const int UNIX_2015_03_01_00_00_00 = 1425168000;

TEST(TimeLib, Year) {
    EXPECT_EQ(year(UNIX_1970_01_01_00_00_00), 1970);
    EXPECT_EQ(year(UNIX_2000_02_29_12_34_56), 2000);
    EXPECT_EQ(year(UNIX_2020_12_31_23_59_59), 2020);
    EXPECT_EQ(year(UNIX_2021_01_01_00_00_00), 2021);
}

TEST(TimeLib, Month) {
    EXPECT_EQ(month(UNIX_1970_01_01_00_00_00), 1);
    EXPECT_EQ(month(UNIX_2000_02_29_12_34_56), 2);
    EXPECT_EQ(month(UNIX_2020_12_31_23_59_59), 12);
    EXPECT_EQ(month(UNIX_2021_01_01_00_00_00), 1);
    EXPECT_EQ(month(UNIX_2016_02_29_00_00_00), 2);
    EXPECT_EQ(month(UNIX_2015_03_01_00_00_00), 3);
}

TEST(TimeLib, Day) {
    EXPECT_EQ(day(UNIX_1970_01_01_00_00_00), 1);
    EXPECT_EQ(day(UNIX_2000_02_29_12_34_56), 29);
    EXPECT_EQ(day(UNIX_2020_12_31_23_59_59), 31);
    EXPECT_EQ(day(UNIX_2021_01_01_00_00_00), 1);
    EXPECT_EQ(day(UNIX_2016_02_29_00_00_00), 29);
    EXPECT_EQ(day(UNIX_2015_03_01_00_00_00), 1);
}

TEST(TimeLib, Hour) {
    EXPECT_EQ(hour(UNIX_1970_01_01_00_00_00), 0);
    EXPECT_EQ(hour(UNIX_2000_02_29_12_34_56), 12);
    EXPECT_EQ(hour(UNIX_2020_12_31_23_59_59), 23);
}

TEST(TimeLib, Minute) {
    EXPECT_EQ(minute(UNIX_1970_01_01_00_00_00), 0);
    EXPECT_EQ(minute(UNIX_2000_02_29_12_34_56), 34);
    EXPECT_EQ(minute(UNIX_2020_12_31_23_59_59), 59);
}

TEST(TimeLib, Second) {
    EXPECT_EQ(second(UNIX_1970_01_01_00_00_00), 0);
    EXPECT_EQ(second(UNIX_2000_02_29_12_34_56), 56);
    EXPECT_EQ(second(UNIX_2020_12_31_23_59_59), 59);
}