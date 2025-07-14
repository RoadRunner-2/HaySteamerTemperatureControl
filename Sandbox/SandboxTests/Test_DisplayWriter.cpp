#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../DisplayWriter.h"

using ::testing::Return;
using ::testing::_;

// Mock Display126x64
class MockDisplay : public Drawer<4> {
public:
    MOCK_METHOD(void, write, (const String (&content)[4]), (override));
};

// Helper ContentProvider mocks
class MockContentProvider {
public:
    MOCK_METHOD(String, call, (), ());
    operator DisplayWriter::ContentProvider() {
        return [this]() { return call(); };
    }
};

TEST(DisplayWriterTest, UpdateAllLinesCallsDrawerWithCorrectContent) {
    MockDisplay mockDisplay;
    MockContentProvider line1, line2, line3, line4;

    EXPECT_CALL(line1, call()).WillOnce(Return("A"));
    EXPECT_CALL(line2, call()).WillOnce(Return("B"));
    EXPECT_CALL(line3, call()).WillOnce(Return("C"));
    EXPECT_CALL(line4, call()).WillOnce(Return("D"));

    DisplayWriter writer(&mockDisplay);
    writer.setAllProvider(line1, line2, line3, line4);

    String expected[4] = { "A", "B", "C", "D" };
    EXPECT_CALL(mockDisplay, write(testing::Truly([&expected](const String (&arr)[4]) {
        for (int i = 0; i < 4; ++i) if (arr[i] != expected[i]) return false;
        return true;
    })));
    writer.updateAllLines();
}

TEST(DisplayWriterTest, SetLineProviderChangesContent) {
    MockDisplay mockDisplay;
    MockContentProvider line1, line2, line3, line4, newLine2;

    EXPECT_CALL(line1, call()).WillRepeatedly(Return("A"));
    EXPECT_CALL(line2, call()).WillRepeatedly(Return("B"));
    EXPECT_CALL(line3, call()).WillRepeatedly(Return("C"));
    EXPECT_CALL(line4, call()).WillRepeatedly(Return("D"));
    EXPECT_CALL(newLine2, call()).WillRepeatedly(Return("X"));

    DisplayWriter writer(&mockDisplay);
    writer.setAllProvider(line1, line2, line3, line4);

    // First update: original content
    String expected1[4] = { "A", "B", "C", "D" };
    EXPECT_CALL(mockDisplay, write(testing::Truly([&expected1](const String (&arr)[4]) {
        for (int i = 0; i < 4; ++i) if (arr[i] != expected1[i]) return false;
        return true;
    })));
    writer.updateAllLines();

    // Change line 1 provider and update again
    writer.setLineProvider(1, newLine2);
    String expected2[4] = { "A", "X", "C", "D" };
    EXPECT_CALL(mockDisplay, write(testing::Truly([&expected2](const String (&arr)[4]) {
        for (int i = 0; i < 4; ++i) if (arr[i] != expected2[i]) return false;
        return true;
    })));
    writer.updateAllLines();
}

TEST(DisplayWriterTest, InvalidLineProviderDoesNotCrash) {
    MockDisplay mockDisplay;
    MockContentProvider line1, line2, line3, line4, invalid;

    EXPECT_CALL(line1, call()).WillRepeatedly(Return("A"));
    EXPECT_CALL(line2, call()).WillRepeatedly(Return("B"));
    EXPECT_CALL(line3, call()).WillRepeatedly(Return("C"));
    EXPECT_CALL(line4, call()).WillRepeatedly(Return("D"));

    DisplayWriter writer(&mockDisplay);
    writer.setAllProvider(line1, line2, line3, line4);
    // Should not throw or crash
    writer.setLineProvider(-1, invalid);
    writer.setLineProvider(4, invalid);
}

TEST(DisplayWriterTest, ChangeToInvalidProviderExpectEmptyString) {
    MockDisplay mockDisplay;
    MockContentProvider line1, line2, line3, line4, invalid;

    EXPECT_CALL(line1, call()).WillRepeatedly(Return("A"));
    EXPECT_CALL(line2, call()).WillRepeatedly(Return("B"));
    EXPECT_CALL(line3, call()).WillRepeatedly(Return("C"));
    EXPECT_CALL(line4, call()).WillRepeatedly(Return("D"));

    DisplayWriter writer(&mockDisplay);
    writer.setAllProvider(line1, line2, line3, line4);

    // First update: original content
    String expected1[4] = { "A", "B", "C", "D" };
    EXPECT_CALL(mockDisplay, write(testing::Truly([&expected1](const String(&arr)[4]) {
        for (int i = 0; i < 4; ++i) if (arr[i] != expected1[i]) return false;
        return true;
    })));
    writer.updateAllLines();

    // Change line 1 provider to invalid and update again
    writer.setLineProvider(1, invalid);
    String expected2[4] = { "A", "", "C", "D" };
    EXPECT_CALL(mockDisplay, write(testing::Truly([&expected2](const String(&arr)[4]) {
        for (int i = 0; i < 4; ++i) if (arr[i] != expected2[i]) return false;
        return true;
    })));
    writer.updateAllLines();
}