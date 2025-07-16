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
    EXPECT_EQ(faults.checkConditions(Status::idle), "");
}

// Single condition true, returns its message
TEST_F(FaultConditionsTest, SingleTrueConditionReturnsMessage) {
    faults.addCondition([](Status) { return true; }, "Fault1");
    EXPECT_EQ(faults.checkConditions(Status::idle), "Fault1");
}

// Single condition false, returns default message
TEST_F(FaultConditionsTest, SingleFalseConditionReturnsDefault) {
    faults.addCondition([](Status) { return false; }, "Fault1");
    EXPECT_EQ(faults.checkConditions(Status::idle), "");
}

// Multiple conditions, first true is returned
TEST_F(FaultConditionsTest, MultipleConditionsFirstTrueReturned) {
    faults.addCondition([](Status) { return false; }, "Fault1");
    faults.addCondition([](Status) { return true; }, "Fault2");
    faults.addCondition([](Status) { return true; }, "Fault3");
    EXPECT_EQ(faults.checkConditions(Status::idle), "Fault2");
}

// Multiple conditions, all false, returns default
TEST_F(FaultConditionsTest, MultipleConditionsAllFalseReturnsDefault) {
    faults.addCondition([](Status) { return false; }, "Fault1");
    faults.addCondition([](Status) { return false; }, "Fault2");
    EXPECT_EQ(faults.checkConditions(Status::idle), "");
}

// Add null condition, should be ignored
TEST_F(FaultConditionsTest, AddNullConditionIgnored) {
    int callCount = 0;
    faults.addCondition(nullptr, "Fault1");
    faults.addCondition([&](Status) { ++callCount; return false; }, "Fault2");
    faults.checkConditions(Status::idle);
    EXPECT_EQ(callCount, 1);
}

// Multiple calls to checkConditions, state is preserved
TEST_F(FaultConditionsTest, MultipleCallsPreserveState) {
    faults.addCondition([](Status) { return false; }, "Fault1");
    EXPECT_EQ(faults.checkConditions(Status::idle), "Default");
    faults.addCondition([](Status) { return true; }, "Fault2");
    EXPECT_EQ(faults.checkConditions(Status::idle), "Fault2");
}

TEST_F(FaultConditionsTest, ConditionTrueForSpecificStates) {
    // Condition returns true only for heating and holding
    faults.addCondition([](Status s) {
        return s == Status::heating || s == Status::holding;
        }, "Active");

    EXPECT_EQ(faults.checkConditions(Status::idle), "");
    EXPECT_EQ(faults.checkConditions(Status::heating), "Active");
    EXPECT_EQ(faults.checkConditions(Status::holding), "Active");
    EXPECT_EQ(faults.checkConditions(Status::done), "");
    EXPECT_EQ(faults.checkConditions(Status::error), "");
}