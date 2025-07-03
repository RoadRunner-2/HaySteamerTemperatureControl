#include "pch.h"
#include "ParameterEditor.h"

// Test fixture for DisplayFormatter
class DisplayFormatterTest : public ::testing::Test {
protected:
    void SetUp() override {
        formatter = new DisplayFormatter();
    }

    void TearDown() override {
        delete formatter;
    }

    DisplayFormatter* formatter;
};

// DisplayFormatter Tests
TEST_F(DisplayFormatterTest, FormatIdleDisplay) {
    String result = formatter->formatIdleDisplay(12, 34, 56, 78);
    EXPECT_EQ(result, "12:34, 56°C, 78min");
}

TEST_F(DisplayFormatterTest, FormatIdleDisplayWithLeadingZeros) {
    String result = formatter->formatIdleDisplay(9, 5, 7, 3);
    EXPECT_EQ(result, "09:05, 7°C, 3min");
}

TEST_F(DisplayFormatterTest, FormatEditDisplayTimeEdit) {
    char buffer[] = "12";
    String result = formatter->formatEditDisplay(ManualEditor::TIME_EDIT, buffer, 2, 0, 0, 25, 30);

    // The result should show "12__" for time (with blinking cursor)
    // The exact format depends on blink state, but should contain the input
    EXPECT_TRUE(result.find("12") != std::string::npos);
    EXPECT_TRUE(result.find("25°C") != std::string::npos);
    EXPECT_TRUE(result.find("30min") != std::string::npos);
}

TEST_F(DisplayFormatterTest, FormatEditDisplayTempEdit) {
    char buffer[] = "2";
    String result = formatter->formatEditDisplay(ManualEditor::TEMP_EDIT, buffer, 1, 12, 34, 0, 30);

    EXPECT_TRUE(result.find("12:34") != std::string::npos);
    EXPECT_TRUE(result.find("2") != std::string::npos);
    EXPECT_TRUE(result.find("°C") != std::string::npos);
    EXPECT_TRUE(result.find("30min") != std::string::npos);
}

TEST_F(DisplayFormatterTest, FormatEditDisplaySpanEdit) {
    char buffer[] = "4";
    String result = formatter->formatEditDisplay(ManualEditor::SPAN_EDIT, buffer, 1, 12, 34, 25, 0);

    EXPECT_TRUE(result.find("12:34") != std::string::npos);
    EXPECT_TRUE(result.find("25°C") != std::string::npos);
    EXPECT_TRUE(result.find("4") != std::string::npos);
    EXPECT_TRUE(result.find("min") != std::string::npos);
}
