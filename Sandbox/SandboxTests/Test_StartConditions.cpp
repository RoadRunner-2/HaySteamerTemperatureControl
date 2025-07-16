#include "gtest/gtest.h"
#include "../../StartConditions.h"

// Test fixture for StartConditions
class StartConditionsTest : public ::testing::Test {
protected:
    StartConditions conditions;
};

// Test: No conditions added, checkAllConditions should return false
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

// Test: Add multiple conditions, all true
TEST_F(StartConditionsTest, MultipleTrueConditionsReturnsTrue) {
    conditions.addCondition([] { return true; });
    conditions.addCondition([] { return true; });
    EXPECT_TRUE(conditions.checkAllConditions());
}