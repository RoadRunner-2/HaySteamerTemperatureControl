#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../KeypadReader.h"

// Mock Sensor implementation
class MockKeypad : public Sensor<char> {
public:
    MOCK_METHOD(char, read, (), (override));
    MOCK_METHOD(void, initialize, (), (override));
};

using ::testing::Return;

TEST(KeypadReaderTest, UpdateStoresKeypadValue) {
    MockKeypad mockKeypad;
    KeypadReader reader(&mockKeypad);

    EXPECT_CALL(mockKeypad, read()).WillOnce(Return(4));
    reader.update();
    EXPECT_EQ(reader.getLatestValue(), 4);
}

TEST(KeypadReaderTest, GetLatestValueReturnsInitialValue) {
    MockKeypad mockKeypad;
    KeypadReader reader(&mockKeypad);
    // No update called yet, should be 0 (default)
    EXPECT_EQ(reader.getLatestValue(), 0);
}

TEST(KeypadReaderTest, MultipleUpdatesStoreLatestValue) {
    MockKeypad mockKeypad;
    KeypadReader reader(&mockKeypad);

    EXPECT_CALL(mockKeypad, read())
        .WillOnce(Return('A'))
        .WillOnce(Return('#'));

    reader.update();
    EXPECT_EQ(reader.getLatestValue(), 'A');
    reader.update();
    EXPECT_EQ(reader.getLatestValue(), '#');
}