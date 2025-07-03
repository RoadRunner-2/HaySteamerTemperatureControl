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

// ManualEditor Tests
TEST_F(ManualEditorTest, InitialState) {
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_STREQ(editor->getInputBuffer(), "");
    EXPECT_EQ(editor->getInputPos(), 0);
}

TEST_F(ManualEditorTest, SelectModeA) {
    editor->selectMode('A');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::TIME_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

TEST_F(ManualEditorTest, SelectModeB) {
    editor->selectMode('B');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::TEMP_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

TEST_F(ManualEditorTest, SelectModeC) {
    editor->selectMode('C');
    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::SPAN_EDIT);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}

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

TEST_F(ManualEditorTest, TimeEditInvalidFirstHourDigit) {
    editor->selectMode('A');
    EXPECT_FALSE(editor->processDigit('3'));  // Invalid first hour digit (>2)
    EXPECT_EQ(editor->getInputPos(), 0);
}

TEST_F(ManualEditorTest, TimeEditInvalidSecondHourDigit) {
    editor->selectMode('A');
    editor->processDigit('2');
    EXPECT_FALSE(editor->processDigit('4'));  // Invalid when first digit is 2 (>23)
    EXPECT_EQ(editor->getInputPos(), 1);
}

TEST_F(ManualEditorTest, TimeEditInvalidFirstMinuteDigit) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');
    EXPECT_FALSE(editor->processDigit('6'));  // Invalid first minute digit (>5)
    EXPECT_EQ(editor->getInputPos(), 2);
}

TEST_F(ManualEditorTest, TempEditValidInput) {
    editor->selectMode('B');

    EXPECT_FALSE(editor->processDigit('2'));
    EXPECT_EQ(editor->getInputPos(), 1);
    EXPECT_STREQ(editor->getInputBuffer(), "2");

    EXPECT_TRUE(editor->processDigit('5'));
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "25");
}

TEST_F(ManualEditorTest, TempEditMaxValue) {
    editor->selectMode('B');
    editor->processDigit('9');
    EXPECT_TRUE(editor->processDigit('9'));  // 99 is valid
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "99");
}

TEST_F(ManualEditorTest, SpanEditValidInput) {
    editor->selectMode('C');

    EXPECT_FALSE(editor->processDigit('3'));
    EXPECT_EQ(editor->getInputPos(), 1);
    EXPECT_STREQ(editor->getInputBuffer(), "3");

    EXPECT_TRUE(editor->processDigit('0'));
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "30");
}

TEST_F(ManualEditorTest, SpanEditMaxValue) {
    editor->selectMode('C');
    editor->processDigit('6');
    EXPECT_TRUE(editor->processDigit('0'));  // 60 is valid
    EXPECT_EQ(editor->getInputPos(), 2);
    EXPECT_STREQ(editor->getInputBuffer(), "60");
}

TEST_F(ManualEditorTest, SpanEditInvalidAfterSix) {
    editor->selectMode('C');
    editor->processDigit('6');
    EXPECT_FALSE(editor->processDigit('1'));  // 61 is invalid
    EXPECT_EQ(editor->getInputPos(), 1);
}

TEST_F(ManualEditorTest, ProcessDigitWithoutModeSelection) {
    EXPECT_FALSE(editor->processDigit('1'));
    EXPECT_EQ(editor->getInputPos(), 0);
}

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

TEST_F(ManualEditorTest, AbortEdit) {
    editor->selectMode('A');
    editor->processDigit('1');
    editor->processDigit('2');

    editor->abortEdit();

    EXPECT_EQ(editor->getCurrentMode(), ManualEditor::NONE);
    EXPECT_EQ(editor->getInputPos(), 0);
    EXPECT_STREQ(editor->getInputBuffer(), "");
}