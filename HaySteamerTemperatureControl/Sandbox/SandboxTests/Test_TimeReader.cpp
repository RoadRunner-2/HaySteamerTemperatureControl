#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../TimeReader.h"

using ::testing::Return;

class MockNTPClock : public Sensor<time_t> {
public:
    MOCK_METHOD(time_t, read, (), (override));
};

TEST(TimeReaderTest, ReturnsMidnight) {
    MockNTPClock mockClock;
    TimeReader reader(&mockClock);
    EXPECT_CALL(mockClock, read()).WillOnce(Return(0)); // 1970-01-01 00:00:00
    reader.update();
    EXPECT_EQ(reader.getTimeOfDayInMinutes(), 0);
    EXPECT_EQ(reader.getDisplayString(), "00:00 01.01.1970");
}

TEST(TimeReaderTest, ReturnsNoon) {
    MockNTPClock mockClock;
    TimeReader reader(&mockClock);
    // 12:00:00 is 12*60*60 = 43200 seconds
    EXPECT_CALL(mockClock, read()).WillOnce(Return(43200));
    reader.update();
    EXPECT_EQ(reader.getTimeOfDayInMinutes(), 12 * 60);
    EXPECT_EQ(reader.getDisplayString(), "12:00 01.01.1970");
}

TEST(TimeReaderTest, ReturnsSpecificDateTime) {
    MockNTPClock mockClock;
    TimeReader reader(&mockClock);
    // 2000-02-29 12:34:56 UTC = 951827696
    EXPECT_CALL(mockClock, read()).WillOnce(Return(951827696));
    reader.update();
    EXPECT_EQ(reader.getDisplayString(), "12:34 29.02.2000");
}

TEST(TimeReaderTest, ReturnsEndOfYear) {
    MockNTPClock mockClock;
    TimeReader reader(&mockClock);
    // 2020-12-31 23:59:59 UTC = 1609459199
    EXPECT_CALL(mockClock, read()).WillOnce(Return(1609459199));
    reader.update();
    EXPECT_EQ(reader.getDisplayString(), "23:59 31.12.2020");
}

TEST(TimeReaderTest, MultipleUpdates) {
    MockNTPClock mockClock;
    TimeReader reader(&mockClock);
    EXPECT_CALL(mockClock, read())
        .WillOnce(Return(0))
        .WillOnce(Return(1609459200)); // 2021-01-01 00:00:00
    reader.update();
    EXPECT_EQ(reader.getDisplayString(), "00:00 01.01.1970");
    reader.update();
    EXPECT_EQ(reader.getDisplayString(), "00:00 01.01.2021");
}
