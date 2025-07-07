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

// Tests initial parameter values after construction
TEST_F(ParameterEditorTest, InitialValues) {
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
    EXPECT_EQ(editor->getTimeInMinutes(), 720);
    EXPECT_EQ(editor->getTemperature(), 20);
    EXPECT_EQ(editor->getTimeSpan(), 30);
}

// Tests setting valid time values
TEST_F(ParameterEditorTest, SetTime) {
    editor->setTime(15, 45);
    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 45);
    EXPECT_EQ(editor->getTimeInMinutes(), 945);
}

// Tests setting invalid hour (should not change)
TEST_F(ParameterEditorTest, SetTimeInvalidHours) {
    editor->setTime(25, 30);  // Invalid hours
    EXPECT_EQ(editor->getTimeHours(), 12);  // Should remain unchanged
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Tests setting invalid minutes (should not change)
TEST_F(ParameterEditorTest, SetTimeInvalidMinutes) {
    editor->setTime(10, 65);  // Invalid minutes
    EXPECT_EQ(editor->getTimeHours(), 12);  // Should remain unchanged
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Tests setting valid temperature
TEST_F(ParameterEditorTest, SetTemperature) {
    editor->setTemperature(25);
    EXPECT_EQ(editor->getTemperature(), 25);
}

// Tests setting invalid temperature (should not change)
TEST_F(ParameterEditorTest, SetTemperatureInvalid) {
    editor->setTemperature(100);  // Invalid temperature
    EXPECT_EQ(editor->getTemperature(), 20);  // Should remain unchanged
}

// Tests setting valid time span
TEST_F(ParameterEditorTest, SetTimeSpan) {
    editor->setTimeSpan(45);
    EXPECT_EQ(editor->getTimeSpan(), 45);
}

// Tests setting invalid time span (should not change)
TEST_F(ParameterEditorTest, SetTimeSpanInvalid) {
    editor->setTimeSpan(65);  // Invalid span
    EXPECT_EQ(editor->getTimeSpan(), 30);  // Should remain unchanged
}

// Tests processKey for mode selection (should enter edit mode)
TEST_F(ParameterEditorTest, ProcessKeyModeSelection) {
    editor->processKey('A');
    String display = editor->getDisplayString();
    EXPECT_TRUE(display.find("_") != std::string::npos);
}

// Tests processKey for digit input and commit (time edit)
TEST_F(ParameterEditorTest, ProcessKeyDigitInput) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');
    editor->processKey('5');
    editor->processKey('3');
    editor->processKey('0');  // Complete time input: 15:30

    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 30);
}

// Tests processKey for aborting an edit
TEST_F(ParameterEditorTest, ProcessKeyAbort) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');
    editor->processKey('*');  // Abort

    String display = editor->getDisplayString();
    EXPECT_TRUE(display.find("12:00") != std::string::npos);
}

// Tests processKey for temperature edit
TEST_F(ParameterEditorTest, ProcessKeyTempEdit) {
    editor->processKey('B');  // Enter temp edit mode
    editor->processKey('2');
    editor->processKey('5');  // Complete temp input: 25

    EXPECT_EQ(editor->getTemperature(), 25);
}

// Tests processKey for span edit
TEST_F(ParameterEditorTest, ProcessKeySpanEdit) {
    editor->processKey('C');  // Enter span edit mode
    editor->processKey('4');
    editor->processKey('5');  // Complete span input: 45

    EXPECT_EQ(editor->getTimeSpan(), 45);
}

// Tests getDisplayString in idle state
TEST_F(ParameterEditorTest, GetDisplayStringIdle) {
    String display = editor->getDisplayString();
    EXPECT_EQ(display, "12:00, 20°C, 30min");
}

// Tests getDisplayString in editing state (partial input)
TEST_F(ParameterEditorTest, GetDisplayStringEditing) {
    editor->processKey('A');  // Enter time edit mode
    editor->processKey('1');

    String display = editor->getDisplayString();
    EXPECT_TRUE(display.find("1") != std::string::npos);
    EXPECT_TRUE(display.find("_") != std::string::npos);
}

// Tests a complete workflow: time, temp, and span edits
TEST_F(ParameterEditorTest, CompleteWorkflow) {
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

// Edge case: maximum valid time input
TEST_F(ParameterEditorTest, MaxTimeInput) {
    editor->processKey('A');
    editor->processKey('2');
    editor->processKey('3');
    editor->processKey('5');
    editor->processKey('9');  // 23:59

    EXPECT_EQ(editor->getTimeHours(), 23);
    EXPECT_EQ(editor->getTimeMinutes(), 59);
}

// Edge case: maximum valid temperature input
TEST_F(ParameterEditorTest, MaxTempInput) {
    editor->processKey('B');
    editor->processKey('9');
    editor->processKey('9');  // 99°C

    EXPECT_EQ(editor->getTemperature(), 99);
}

// Edge case: maximum valid span input
TEST_F(ParameterEditorTest, MaxSpanInput) {
    editor->processKey('C');
    editor->processKey('6');
    editor->processKey('0');  // 60min

    EXPECT_EQ(editor->getTimeSpan(), 60);
}

// Tests processKey with invalid mode key (should not change state)
TEST_F(ParameterEditorTest, ProcessKeyInvalidMode) {
    String before = editor->getDisplayString();
    editor->processKey('X'); // Invalid mode
    String after = editor->getDisplayString();
    EXPECT_EQ(before, after);
}

// Tests processKey with invalid digit in edit mode (should not change input)
TEST_F(ParameterEditorTest, ProcessKeyInvalidDigitInEdit) {
    editor->processKey('A');
    editor->processKey('3'); // Invalid first hour digit
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Tests that processKey ignores input when not in edit mode and not a mode key
TEST_F(ParameterEditorTest, ProcessKeyIgnoredWhenIdle) {
    String before = editor->getDisplayString();
    editor->processKey('1'); // Not in edit mode
    String after = editor->getDisplayString();
    EXPECT_EQ(before, after);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}