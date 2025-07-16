#include "gtest/gtest.h"
#include "../../FaultConditions.h"

// Helper for String type compatibility
using String = std::string;

// Test fixture for FaultConditions
class FaultConditionsTest : public ::testing::Test {
protected:
    FaultConditions faults;
};

// No conditions, default message is empty
TEST_F(FaultConditionsTest, NoConditionsReturnsDefaultMessage) {
    EXPECT_EQ(faults.checkConditions(), "");
}

// No conditions, custom default message
TEST_F(FaultConditionsTest, CustomDefaultMessageIsReturned) {
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Default");
}

// Single condition true, returns its message
TEST_F(FaultConditionsTest, SingleTrueConditionReturnsMessage) {
    faults.addCondition([] { return true; }, "Fault1");
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Fault1");
}

// Single condition false, returns default message
TEST_F(FaultConditionsTest, SingleFalseConditionReturnsDefault) {
    faults.addCondition([] { return false; }, "Fault1");
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Default");
}

// Multiple conditions, first true is returned
TEST_F(FaultConditionsTest, MultipleConditionsFirstTrueReturned) {
    faults.addCondition([] { return false; }, "Fault1");
    faults.addCondition([] { return true; }, "Fault2");
    faults.addCondition([] { return true; }, "Fault3");
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Fault2");
}

// Multiple conditions, all false, returns default
TEST_F(FaultConditionsTest, MultipleConditionsAllFalseReturnsDefault) {
    faults.addCondition([] { return false; }, "Fault1");
    faults.addCondition([] { return false; }, "Fault2");
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Default");
}

// Add null condition, should be ignored
TEST_F(FaultConditionsTest, AddNullConditionIgnored) {
    int callCount = 0;
    faults.addCondition(nullptr, "Fault1");
    faults.addCondition([&] { ++callCount; return false; }, "Fault2");
    faults.addDefaultMessage([]() { return String("Default"); });
    faults.checkConditions();
    EXPECT_EQ(callCount, 1);
}

// Add null default message, should not change default
TEST_F(FaultConditionsTest, AddNullDefaultMessageIgnored) {
    faults.addDefaultMessage(nullptr);
    EXPECT_EQ(faults.checkConditions(), "");
}

// Multiple calls to checkConditions, state is preserved
TEST_F(FaultConditionsTest, MultipleCallsPreserveState) {
    faults.addCondition([] { return false; }, "Fault1");
    faults.addDefaultMessage([]() { return String("Default"); });
    EXPECT_EQ(faults.checkConditions(), "Default");
    faults.addCondition([] { return true; }, "Fault2");
    EXPECT_EQ(faults.checkConditions(), "Fault2");
}