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

// Tests formatIdleDisplay with typical values
TEST_F(DisplayFormatterTest, FormatIdleDisplay) {
    String result = formatter->formatIdleDisplay(12, 34, 56, 78);
    EXPECT_EQ(result, "12:34, 56C, 78min");
}

// Tests formatIdleDisplay with leading zeros for hours and minutes
TEST_F(DisplayFormatterTest, FormatIdleDisplayWithLeadingZeros) {
    String result = formatter->formatIdleDisplay(9, 5, 7, 3);
    EXPECT_EQ(result, "09:05, 7C, 3min");
}

// Tests formatEditDisplay in TIME_EDIT mode
TEST_F(DisplayFormatterTest, FormatEditDisplayTimeEdit) {
    char buffer[] = "12";
    String result = formatter->formatEditDisplay(ManualEditor::TIME_EDIT, buffer, 2, 0, 0, 25, 30);
    // The result should show "12" for time (with blinking cursor)
    EXPECT_EQ(result, "12: _, 25C, 30min");
}

// Tests formatEditDisplay in TEMP_EDIT mode
TEST_F(DisplayFormatterTest, FormatEditDisplayTempEdit) {
    char buffer[] = "2";
    String result = formatter->formatEditDisplay(ManualEditor::TEMP_EDIT, buffer, 1, 12, 34, 0, 30);
    EXPECT_EQ(result, "12:34, 2 C, 30min");
}

// Tests formatEditDisplay in SPAN_EDIT mode
TEST_F(DisplayFormatterTest, FormatEditDisplaySpanEdit) {
    char buffer[] = "4";
    String result = formatter->formatEditDisplay(ManualEditor::SPAN_EDIT, buffer, 1, 12, 34, 25, 0);
    EXPECT_EQ(result, "12:34, 25C, 4 min");
}

// Tests formatEditDisplay in NONE mode (should fallback to idle display)
TEST_F(DisplayFormatterTest, FormatEditDisplayNoneMode) {
    char buffer[] = "";
    String result = formatter->formatEditDisplay(ManualEditor::NONE, buffer, 0, 1, 2, 3, 4);
    EXPECT_EQ(result, "01:02, 3C, 4min");
}
