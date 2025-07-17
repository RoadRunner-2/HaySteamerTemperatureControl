#include "gtest/gtest.h"
#include "../../StartConditions.h"

// Test fixture for StartConditions
class StartConditionsTest : public ::testing::Test {
protected:
    StartConditions conditions;
};

// Test: timerCondition returns false if time < start
TEST_F(StartConditionsTest, TimerConditionFalse) {
    int time = 100;
    int start = 200;
    conditions.setGetTimeOfDayInMinutes([&] { return time; });
    conditions.setGetStartTimeInMinutes([&] { return start; });
    EXPECT_FALSE(conditions.timerCondition());
}

// Test: timerCondition returns true if time >= start
TEST_F(StartConditionsTest, TimerConditionTrue) {
    int time = 300;
    int start = 200;
    conditions.setGetTimeOfDayInMinutes([&] { return time; });
    conditions.setGetStartTimeInMinutes([&] { return start; });
    EXPECT_TRUE(conditions.timerCondition());
}

// Test: timerCondition returns true if time == start
TEST_F(StartConditionsTest, TimerConditionEdge) {
    int time = 200;
    int start = 200;
    conditions.setGetTimeOfDayInMinutes([&] { return time; });
    conditions.setGetStartTimeInMinutes([&] { return start; });
    EXPECT_TRUE(conditions.timerCondition());
}

// Test: checkAllConditions returns false if no conditions are added
TEST_F(StartConditionsTest, NoConditionsReturnsFalse) {
    EXPECT_FALSE(conditions.checkAllConditions());
}

// Test: Add a single condition that returns true
TEST_F(StartConditionsTest, SingleTrueConditionReturnsTrue) {
    conditions.addCondition([] { return true; });
    EXPECT_TRUE(conditions.checkAllConditions());
}

// Test: Add a single condition that returns false
TEST_F(StartConditionsTest, SingleFalseConditionReturnsFalse) {
    conditions.addCondition([] { return false; });
    EXPECT_FALSE(conditions.checkAllConditions());
}

// Test: Multiple conditions, all false
TEST_F(StartConditionsTest, MultipleFalseConditionsReturnsFalse) {
    conditions.addCondition([] { return false; });
    conditions.addCondition([] { return false; });
    EXPECT_FALSE(conditions.checkAllConditions());
}

// Test: Multiple conditions, one true
TEST_F(StartConditionsTest, MultipleConditionsOneTrueReturnsTrue) {
    conditions.addCondition([] { return false; });
    conditions.addCondition([] { return true; });
    conditions.addCondition([] { return false; });
    EXPECT_TRUE(conditions.checkAllConditions());
}

// Test: Add empty/null condition (should be ignored)
TEST_F(StartConditionsTest, AddEmptyConditionIgnored) {
    int callCount = 0;
    conditions.addCondition(nullptr); // Should be ignored
    conditions.addCondition([&] { ++callCount; return false; });
    conditions.checkAllConditions();
    EXPECT_EQ(callCount, 1);
}

// Test: Multiple true conditions
TEST_F(StartConditionsTest, MultipleTrueConditionsReturnsTrue) {
    conditions.addCondition([] { return true; });
    conditions.addCondition([] { return true; });
    EXPECT_TRUE(conditions.checkAllConditions());
}