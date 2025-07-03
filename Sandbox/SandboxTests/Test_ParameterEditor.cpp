#include "pch.h"
#include "ParameterEditor.h"

// Test fixture for ParameterEditor
class ParameterEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        editor = new ParameterEditor();
    }

    void TearDown() override {
        delete editor;
    }

    ParameterEditor* editor;
};

// ParameterEditor Tests
TEST_F(ParameterEditorTest, InitialValues) {
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
    EXPECT_EQ(editor->getTimeInMinutes(), 720);
    EXPECT_EQ(editor->getTemperature(), 20);
    EXPECT_EQ(editor->getTimeSpan(), 30);
}

TEST_F(ParameterEditorTest, SetTime) {
    editor->setTime(15, 45);
    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 45);
    EXPECT_EQ(editor->getTimeInMinutes(), 945);
}

TEST_F(ParameterEditorTest, SetTimeInvalidHours) {
    editor->setTime(25, 30);  // Invalid hours
    EXPECT_EQ(editor->getTimeHours(), 12);  // Should remain unchanged
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

TEST_F(ParameterEditorTest, SetTimeInvalidMinutes) {
    editor->setTime(10, 65);  // Invalid minutes
    EXPECT_EQ(editor->getTimeHours(), 12);  // Should remain unchanged
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

TEST_F(ParameterEditorTest, SetTemperature) {
    editor->setTemperature(25);
    EXPECT_EQ(editor->getTemperature(), 25);
}

TEST_F(ParameterEditorTest, SetTemperatureInvalid) {
    editor->setTemperature(100);  // Invalid temperature
    EXPECT_EQ(editor->getTemperature(), 20);  // Should remain unchanged
}

TEST_F(ParameterEditorTest, SetTimeSpan) {
    editor->setTimeSpan(45);
    EXPECT_EQ(editor->getTimeSpan(), 45);
}

TEST_F(ParameterEditorTest, SetTimeSpanInvalid) {
    editor->setTimeSpan(65);  // Invalid span
    EXPECT_EQ(editor->getTimeSpan(), 30);  // Should remain unchanged
}

TEST_F(ParameterEditorTest, ProcessKeyModeSelection) {
    editor->processKey('A');
    String display = editor->getDisplayString();

    // Should be in time edit mode - display should contain underscores
    EXPECT_TRUE(display.find("_") != std::string::npos);
}

TEST_F(ParameterEditorTest, ProcessKeyDigitInput) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');
    editor->processKey('5');
    editor->processKey('3');
    editor->processKey('0');  // Complete time input: 15:30

    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 30);
}

TEST_F(ParameterEditorTest, ProcessKeyAbort) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');
    editor->processKey('*');  // Abort

    String display = editor->getDisplayString();
    // Should be back to idle display
    EXPECT_TRUE(display.find("12:00") != std::string::npos);
}

TEST_F(ParameterEditorTest, ProcessKeyTempEdit) {
    editor->processKey('B');  // Enter temp edit mode
    editor->processKey('2');
    editor->processKey('5');  // Complete temp input: 25

    EXPECT_EQ(editor->getTemperature(), 25);
}

TEST_F(ParameterEditorTest, ProcessKeySpanEdit) {
    editor->processKey('C');  // Enter span edit mode
    editor->processKey('4');
    editor->processKey('5');  // Complete span input: 45

    EXPECT_EQ(editor->getTimeSpan(), 45);
}

TEST_F(ParameterEditorTest, GetDisplayStringIdle) {
    String display = editor->getDisplayString();
    EXPECT_EQ(display, "12:00, 20°C, 30min");
}

TEST_F(ParameterEditorTest, GetDisplayStringEditing) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');

    String display = editor->getDisplayString();
    // Should show partial input with cursor
    EXPECT_TRUE(display.find("1") != std::string::npos);
    EXPECT_TRUE(display.find("_") != std::string::npos);
}

TEST_F(ParameterEditorTest, CompleteWorkflow) {
    // Test complete workflow: set time, temp, and span
    editor->processKey('A');  // Time edit
    editor->processKey('0');
    editor->processKey('8');
    editor->processKey('1');
    editor->processKey('5');  // 08:15

    editor->processKey('B');  // Temp edit
    editor->processKey('2');
    editor->processKey('2');  // 22°C

    editor->processKey('C');  // Span edit
    editor->processKey('4');
    editor->processKey('5');  // 45min

    EXPECT_EQ(editor->getTimeHours(), 8);
    EXPECT_EQ(editor->getTimeMinutes(), 15);
    EXPECT_EQ(editor->getTemperature(), 22);
    EXPECT_EQ(editor->getTimeSpan(), 45);

    String display = editor->getDisplayString();
    EXPECT_EQ(display, "08:15, 22°C, 45min");
}

// Edge case tests
TEST_F(ParameterEditorTest, MaxTimeInput) {
    editor->processKey('A');
    editor->processKey('2');
    editor->processKey('3');
    editor->processKey('5');
    editor->processKey('9');  // 23:59

    EXPECT_EQ(editor->getTimeHours(), 23);
    EXPECT_EQ(editor->getTimeMinutes(), 59);
}

TEST_F(ParameterEditorTest, MaxTempInput) {
    editor->processKey('B');
    editor->processKey('9');
    editor->processKey('9');  // 99°C

    EXPECT_EQ(editor->getTemperature(), 99);
}

TEST_F(ParameterEditorTest, MaxSpanInput) {
    editor->processKey('C');
    editor->processKey('6');
    editor->processKey('0');  // 60min

    EXPECT_EQ(editor->getTimeSpan(), 60);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}