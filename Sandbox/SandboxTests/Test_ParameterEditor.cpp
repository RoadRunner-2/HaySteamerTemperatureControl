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

// Tests setCharacterProvider and update (simulates input stream)
TEST_F(ParameterEditorTest, SetCharacterProviderAndUpdate) {
    std::string input = "A1530";
    size_t idx = 0;
    editor->setCharacterProvider([&]() {
        if (idx < input.size()) return input[idx++];
        return '\0';
    });
    for (size_t i = 0; i < input.size(); ++i) {
        editor->update();
    }
    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 30);
}

// Tests update does nothing if provider returns '\0'
TEST_F(ParameterEditorTest, UpdateWithNoInput) {
    editor->setCharacterProvider([]() { return '\0'; });
    editor->update();
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Tests a complete workflow using setCharacterProvider and update
TEST_F(ParameterEditorTest, CompleteWorkflowWithProvider) {
    std::string input = "A0815B22C45";
    size_t idx = 0;
    editor->setCharacterProvider([&]() {
        if (idx < input.size()) return input[idx++];
        return '\0';
    });
    for (size_t i = 0; i < input.size(); ++i) {
        editor->update();
    }
    EXPECT_EQ(editor->getTimeHours(), 8);
    EXPECT_EQ(editor->getTimeMinutes(), 15);
    EXPECT_EQ(editor->getTemperature(), 22);
    EXPECT_EQ(editor->getTimeSpan(), 45);
    String display = editor->getDisplayString();
    EXPECT_EQ(display, "08:15, 22C, 45min");
}

// Tests that update can be called multiple times with partial input
TEST_F(ParameterEditorTest, UpdatePartialInput) {
    std::string input = "A1";
    size_t idx = 0;
    editor->setCharacterProvider([&]() {
        if (idx < input.size()) return input[idx++];
        return '\0';
    });
    for (size_t i = 0; i < input.size(); ++i) {
        editor->update();
    }
    String display = editor->getDisplayString();
    EXPECT_TRUE(display.find("1") != std::string::npos);
    EXPECT_TRUE(display.find("_") != std::string::npos);
}

// Tests that update ignores input when provider is not set
TEST_F(ParameterEditorTest, UpdateWithoutProvider) {
    editor->update();
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Test changing the character provider after initial set
TEST_F(ParameterEditorTest, ChangeCharacterProviderMidSession) {
    std::string input1 = "A15";
    std::string input2 = "30";
    size_t idx1 = 0, idx2 = 0;
    editor->setCharacterProvider([&]() {
        if (idx1 < input1.size()) return input1[idx1++];
        return '\0';
    });
    for (size_t i = 0; i < input1.size(); ++i) editor->update();
    // Change provider mid-edit
    editor->setCharacterProvider([&]() {
        if (idx2 < input2.size()) return input2[idx2++];
        return '\0';
    });
    for (size_t i = 0; i < input2.size(); ++i) editor->update();
    EXPECT_EQ(editor->getTimeHours(), 15);
    EXPECT_EQ(editor->getTimeMinutes(), 30);
}

// Test min/max valid values for time, temperature, and span
TEST_F(ParameterEditorTest, SetTimeMinMaxValid) {
    editor->setTime(0, 0);
    EXPECT_EQ(editor->getTimeHours(), 0);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
    editor->setTime(23, 59);
    EXPECT_EQ(editor->getTimeHours(), 23);
    EXPECT_EQ(editor->getTimeMinutes(), 59);
}

TEST_F(ParameterEditorTest, SetTemperatureMinMaxValid) {
    editor->setTemperature(0);
    EXPECT_EQ(editor->getTemperature(), 0);
    editor->setTemperature(99);
    EXPECT_EQ(editor->getTemperature(), 99);
}

TEST_F(ParameterEditorTest, SetTimeSpanMinMaxValid) {
    editor->setTimeSpan(0);
    EXPECT_EQ(editor->getTimeSpan(), 0);
    editor->setTimeSpan(60);
    EXPECT_EQ(editor->getTimeSpan(), 60);
}

// Test just-out-of-bounds values for time, temperature, and span
TEST_F(ParameterEditorTest, SetTimeJustOutOfBounds) {
    editor->setTime(-1, 0);
    EXPECT_EQ(editor->getTimeHours(), 12);
    editor->setTime(24, 0);
    EXPECT_EQ(editor->getTimeHours(), 12);
    editor->setTime(0, -1);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
    editor->setTime(0, 60);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

TEST_F(ParameterEditorTest, SetTemperatureJustOutOfBounds) {
    editor->setTemperature(-1);
    EXPECT_EQ(editor->getTemperature(), 20);
    editor->setTemperature(100);
    EXPECT_EQ(editor->getTemperature(), 20);
}

TEST_F(ParameterEditorTest, SetTimeSpanJustOutOfBounds) {
    editor->setTimeSpan(-1);
    EXPECT_EQ(editor->getTimeSpan(), 30);
    editor->setTimeSpan(61);
    EXPECT_EQ(editor->getTimeSpan(), 30);
}

// Test repeated update() calls with no input
TEST_F(ParameterEditorTest, RepeatedUpdateWithNoInput) {
    editor->setCharacterProvider([]() { return '\0'; });
    for (int i = 0; i < 10; ++i) {
        editor->update();
    }
    EXPECT_EQ(editor->getTimeHours(), 12);
    EXPECT_EQ(editor->getTimeMinutes(), 0);
}

// Test getTimeInMinutes for edge values
TEST_F(ParameterEditorTest, GetTimeInMinutesEdgeCases) {
    editor->setTime(0, 0);
    EXPECT_EQ(editor->getTimeInMinutes(), 0);
    editor->setTime(23, 59);
    EXPECT_EQ(editor->getTimeInMinutes(), 23 * 60 + 59);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}