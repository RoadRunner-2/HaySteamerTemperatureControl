#include "pch.h"
#include "ParameterEditor.h"

// Test fixture for ManualEditor
class ManualEditorTest : public ::testing::Test {
protected:
    void SetUp() override {
        editor = new ManualEditor();
    }

    void TearDown() override {
        delete editor;
    }

    ManualEditor* editor;
};

// Verifies the initial state of ManualEditor after construction.
TEST_F(ManualEditorTest, InitialState) {
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_STREQ(editor->getInputBuffer(), "");
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Checks that selecting mode 'A' sets TIME_EDIT mode and resets the buffer and position.
TEST_F(ManualEditorTest, SelectModeA) {
    editor->selectMode('A');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::TIME_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Checks that selecting mode 'B' sets TEMP_EDIT mode and resets the buffer and position.
TEST_F(ManualEditorTest, SelectModeB) {
    editor->selectMode('B');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::TEMP_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Checks that selecting mode 'C' sets SPAN_EDIT mode and resets the buffer and position.
TEST_F(ManualEditorTest, SelectModeC) {
    editor->selectMode('C');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::SPAN_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Ensures that selecting a new mode aborts the current edit and resets the buffer and position.
TEST_F(ManualEditorTest, SelectModeAbortsCurrentEdit) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');
    EXPECT_EQ(editor->getInputPos(), 2);

    editor->selectMode('B');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::TEMP_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Tests valid time input sequence and verifies buffer and completion state after each digit.
TEST_F(ManualEditorTest, TimeEditValidInput) {
    editor->selectMode('A');

    EXPECT_FALSE(editor->processDigit('1'));  // First hour digit
    EXPECT_EQ(editor->getInputPos(), 1);
    EXPECT_STREQ(editor->getInputBuffer(), "1");

    EXPECT_FALSE(editor->processDigit('2'));  // Second hour digit
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "12");

    EXPECT_FALSE(editor->processDigit('3'));  // First minute digit
    EXPECT_EQ(editor->getInputPos(), 3);
    EXPECT_STREQ(editor->getInputBuffer(), "123");

    EXPECT_TRUE(editor->processDigit('4'));  // Second minute digit - should complete
    EXPECT_EQ(editor->getInputPos(), 4);
    EXPECT_STREQ(editor->getInputBuffer(), "1234");
}

// Ensures invalid first hour digit is rejected in TIME_EDIT mode.
TEST_F(ManualEditorTest, TimeEditInvalidFirstHourDigit) {
    editor->selectMode('A');
    EXPECT_FALSE(editor->processDigit('3'));  // Invalid first hour digit (>2)
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Ensures invalid second hour digit is rejected when first digit is '2' in TIME_EDIT mode.
TEST_F(ManualEditorTest, TimeEditInvalidSecondHourDigit) {
    editor->selectMode('A');
    editor->processDigit('2');
    EXPECT_FALSE(editor->processDigit('4'));  // Invalid when first digit is 2 (>23)
    EXPECT_EQ(editor->getInputPos(), 1);
}

// Ensures invalid first minute digit is rejected in TIME_EDIT mode.
TEST_F(ManualEditorTest, TimeEditInvalidFirstMinuteDigit) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');
    EXPECT_FALSE(editor->processDigit('6'));  // Invalid first minute digit (>5)
    EXPECT_EQ(editor->getInputPos(), 2);
}

// Tests valid temperature input and verifies buffer and completion state.
TEST_F(ManualEditorTest, TempEditValidInput) {
    editor->selectMode('B');

    EXPECT_FALSE(editor->processDigit('2'));
    EXPECT_EQ(editor->getInputPos(), 1);
    EXPECT_STREQ(editor->getInputBuffer(), "2");

    EXPECT_TRUE(editor->processDigit('5'));
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "25");
}

// Ensures the maximum valid temperature value (99) is accepted.
TEST_F(ManualEditorTest, TempEditMaxValue) {
    editor->selectMode('B');
    editor->processDigit('9');
    EXPECT_TRUE(editor->processDigit('9'));  // 99 is valid
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "99");
}

// Tests valid span input and verifies buffer and completion state.
TEST_F(ManualEditorTest, SpanEditValidInput) {
    editor->selectMode('C');

    EXPECT_FALSE(editor->processDigit('3'));
    EXPECT_EQ(editor->getInputPos(), 1);
    EXPECT_STREQ(editor->getInputBuffer(), "3");

    EXPECT_TRUE(editor->processDigit('0'));
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "30");
}

// Ensures the maximum valid span value (60) is accepted.
TEST_F(ManualEditorTest, SpanEditMaxValue) {
    editor->selectMode('C');
    editor->processDigit('6');
    EXPECT_TRUE(editor->processDigit('0'));  // 60 is valid
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "60");
}

// Ensures invalid span value (>60) is rejected after entering '6' as the first digit.
TEST_F(ManualEditorTest, SpanEditInvalidAfterSix) {
    editor->selectMode('C');
    editor->processDigit('6');
    EXPECT_FALSE(editor->processDigit('1'));  // 61 is invalid
    EXPECT_EQ(editor->getInputPos(), 1);
}

// Ensures that processDigit returns false and does not change state if no mode is selected.
TEST_F(ManualEditorTest, ProcessDigitWithoutModeSelection) {
    EXPECT_FALSE(editor->processDigit('1'));
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Tests committing a valid time edit updates the correct variables and resets the editor state.
TEST_F(ManualEditorTest, CommitTimeEdit) {
    int hours = 0, minutes = 0, temp = 0, span = 0;

    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');
    editor->processDigit('3');
    editor->processDigit('4');

    editor->commitEdit(hours, minutes, temp, span);

    EXPECT_EQ(hours, 12);
    EXPECT_EQ(minutes, 34);
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Tests committing a valid temperature edit updates the correct variable and resets the editor state.
TEST_F(ManualEditorTest, CommitTempEdit) {
    int hours = 0, minutes = 0, temp = 0, span = 0;

    editor->selectMode('B');
    editor->processDigit('2');
    editor->processDigit('5');

    editor->commitEdit(hours, minutes, temp, span);

    EXPECT_EQ(temp, 25);
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Tests committing a valid span edit updates the correct variable and resets the editor state.
TEST_F(ManualEditorTest, CommitSpanEdit) {
    int hours = 0, minutes = 0, temp = 0, span = 0;

    editor->selectMode('C');
    editor->processDigit('4');
    editor->processDigit('5');

    editor->commitEdit(hours, minutes, temp, span);

    EXPECT_EQ(span, 45);
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Tests that abortEdit resets the editor state and clears the buffer.
TEST_F(ManualEditorTest, AbortEdit) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');

    editor->abortEdit();

    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Tests that selecting an invalid mode key leaves the editor in NONE mode and resets state.
TEST_F(ManualEditorTest, SelectModeInvalidKey) {
    editor->selectMode('X');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Tests that commitEdit with incomplete input does not update any variables and resets state.
TEST_F(ManualEditorTest, CommitEditIncompleteInput) {
    int hours = 0, minutes = 0, temp = 0, span = 0;

    editor->selectMode('A');
    editor->processDigit('1'); // Only one digit, incomplete
    editor->commitEdit(hours, minutes, temp, span);

    EXPECT_EQ(hours, 0);
    EXPECT_EQ(minutes, 0);
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
}

// Tests that the input buffer does not accept more digits than allowed (buffer overflow protection).
TEST_F(ManualEditorTest, BufferOverflowProtection) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');
	EXPECT_FALSE(editor->processDigit('3')); // input not complete yet
	EXPECT_TRUE(editor->processDigit('4')); // input complete
    // Try to add extra digit
	EXPECT_TRUE(editor->processDigit('5')); // processDigit still returns true, but last input is ignored
    EXPECT_EQ(editor->getInputPos(), 4); // Should not increment
    EXPECT_STREQ(editor->getInputBuffer(), "1234");
}

// Tests that abortEdit after a completed edit resets the editor state and clears the buffer.
TEST_F(ManualEditorTest, AbortEditAfterCompletion) {
    editor->selectMode('B');
    editor->processDigit('2');
    editor->processDigit('5');
    // Editing complete, now abort
    editor->abortEdit();
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

// Tests that after commitEdit, the editor requires a new mode selection before accepting more input.
TEST_F(ManualEditorTest, StateResetAfterCommitEdit) {
    int hours = 0, minutes = 0, temp = 0, span = 0;
    editor->selectMode('C');
    editor->processDigit('4');
    editor->processDigit('5');
    editor->commitEdit(hours, minutes, temp, span);

    // Try to process digit after commit (should require mode selection)
    EXPECT_FALSE(editor->processDigit('1'));
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
}