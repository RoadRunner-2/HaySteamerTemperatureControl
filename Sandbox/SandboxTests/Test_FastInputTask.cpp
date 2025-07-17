#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../TaskScheduler.h"

// Mock for CyclicModule
class MockCyclicModule : public CyclicModule {
public:
    MOCK_METHOD(void, update, (), (override));
};

// Test fixture for FastInputTask
class FastInputTaskTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a valid interval to avoid clamping
        task = new FastInputTask(100);
    }
    void TearDown() override {
        delete task;
    }
    FastInputTask* task;
};

TEST_F(FastInputTaskTest, EnableSetsEnabledTrue) {
    task->enabled = false;
    task->enable();
    EXPECT_TRUE(task->enabled);
}

TEST_F(FastInputTaskTest, DisableSetsEnabledFalse) {
    task->enabled = true;
    task->disable();
    EXPECT_FALSE(task->enabled);
}

TEST_F(FastInputTaskTest, CycleTaskCallsSuperWhenEnabled) {
    MockCyclicModule m1, m2;
    task->modules.clear();
    task->addModule(&m1);
    task->addModule(&m2);
    task->enabled = true;

    EXPECT_CALL(m1, update()).Times(1);
    EXPECT_CALL(m2, update()).Times(1);

    task->cycleTask();
}

TEST_F(FastInputTaskTest, CycleTaskDoesNotCallSuperWhenDisabled) {
    MockCyclicModule m1, m2;
    task->modules.clear();
    task->addModule(&m1);
    task->addModule(&m2);
    task->enabled = false;

    EXPECT_CALL(m1, update()).Times(0);
    EXPECT_CALL(m2, update()).Times(0);

    task->cycleTask();
}

// Edge: enable/disable multiple times
TEST_F(FastInputTaskTest, EnableDisableToggle) {
    task->enabled = false;
    task->enable();
    EXPECT_TRUE(task->enabled);
    task->disable();
    EXPECT_FALSE(task->enabled);
    task->enable();
    EXPECT_TRUE(task->enabled);
}

// Edge: cycleTask with no modules, enabled
TEST_F(FastInputTaskTest, CycleTaskEnabledNoModules) {
    task->modules.clear();
    task->enabled = true;
    EXPECT_NO_THROW(task->cycleTask());
}

// Edge: cycleTask with no modules, disabled
TEST_F(FastInputTaskTest, CycleTaskDisabledNoModules) {
    task->modules.clear();
    task->enabled = false;
    EXPECT_NO_THROW(task->cycleTask());
}