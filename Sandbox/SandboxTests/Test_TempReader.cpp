#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../TempReader.h"
#include "../Sensor.h"

// Mock Sensor implementation
class MockSensor : public Sensor<int> {
public:
    MOCK_METHOD(int, read, (), (override));
};

using ::testing::Return;

TEST(TempReaderTest, ReturnsInitialValueBeforeUpdate) {
    MockSensor mockSensor;
    TempReader reader(&mockSensor);

    // Before update, lastValue should be 0
    EXPECT_EQ(reader.getLatestValue(), 0);
    EXPECT_EQ(reader.getDisplayString(), "  0C");
}

TEST(TempReaderTest, UpdatesValueFromSensor) {
    MockSensor mockSensor;
    TempReader reader(&mockSensor);

    EXPECT_CALL(mockSensor, read())
        .WillOnce(Return(23));

    reader.update();
    EXPECT_EQ(reader.getLatestValue(), 23);
    EXPECT_EQ(reader.getDisplayString(), " 23C");
}

TEST(TempReaderTest, UpdatesValueMultipleTimes) {
    MockSensor mockSensor;
    TempReader reader(&mockSensor);

    EXPECT_CALL(mockSensor, read())
        .WillOnce(Return(15))
        .WillOnce(Return(-5));

    reader.update();
    EXPECT_EQ(reader.getLatestValue(), 15);
    EXPECT_EQ(reader.getDisplayString(), " 15C");

    reader.update();
    EXPECT_EQ(reader.getLatestValue(), -5);
    EXPECT_EQ(reader.getDisplayString(), " -5C");
}