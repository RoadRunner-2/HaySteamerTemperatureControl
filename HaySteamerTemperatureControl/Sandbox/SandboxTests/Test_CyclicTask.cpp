#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../TaskScheduler.h"

// Mock for CyclicModule
class MockCyclicModule : public CyclicModule {
public:
    MOCK_METHOD(void, update, (), (override));
};

// Testable subclass to expose protected members if needed
struct TestCyclicTask : public CyclicTask {
    using CyclicTask::CyclicTask;
    using CyclicTask::modules;
    using CyclicTask::interval;
    using CyclicTask::nextRun;
};

// Constructor and member initialization
TEST(CyclicTaskTest, ConstructorInitializesMembers) {
    TestCyclicTask task(1234);
    EXPECT_EQ(task.interval, 1234u);
    EXPECT_EQ(task.nextRun, 0u);
    EXPECT_TRUE(task.modules.empty());
}

// initializeTaskTimer sets nextRun correctly
TEST(CyclicTaskTest, InitializeTaskTimerSetsNextRun) {
    TestCyclicTask task(1000);
    task.initializeTaskTimer(500);
    EXPECT_EQ(task.nextRun, 1500u);
}

// isRunScheduled returns false if not scheduled, true if scheduled, and updates nextRun
TEST(CyclicTaskTest, IsRunScheduledReturnsCorrectlyAndUpdatesNextRun) {
    TestCyclicTask task(100);
    task.initializeTaskTimer(1000);
    EXPECT_FALSE(task.isRunScheduled(1050)); // not yet
    EXPECT_TRUE(task.isRunScheduled(1100));  // scheduled
    EXPECT_EQ(task.nextRun, 1200u);
}

// isRunScheduled handles multiple missed intervals
TEST(CyclicTaskTest, IsRunScheduledHandlesMissedIntervals) {
    TestCyclicTask task(100);
    task.initializeTaskTimer(1000);
    EXPECT_TRUE(task.isRunScheduled(1350)); // missed 3 intervals
    EXPECT_EQ(task.nextRun, 1400u);
}

// cycleTask calls update on all modules
TEST(CyclicTaskTest, CycleTaskCallsUpdateOnModules) {
    TestCyclicTask task(1000);
    MockCyclicModule m1, m2;
    task.addModule(&m1);
    task.addModule(&m2);

    EXPECT_CALL(m1, update()).Times(1);
    EXPECT_CALL(m2, update()).Times(1);

    task.cycleTask();
}

// addModule adds modules to the list
TEST(CyclicTaskTest, AddModuleAddsToModules) {
    TestCyclicTask task(1000);
    MockCyclicModule m1, m2;
    EXPECT_TRUE(task.modules.empty());
    task.addModule(&m1);
    task.addModule(&m2);
    EXPECT_EQ(task.modules.size(), 2u);
    EXPECT_EQ(task.modules[0], &m1);
    EXPECT_EQ(task.modules[1], &m2);
}

// Edge: cycleTask with no modules should not throw or crash
TEST(CyclicTaskTest, CycleTaskWithNoModulesDoesNothing) {
    TestCyclicTask task(1000);
    EXPECT_NO_THROW(task.cycleTask());
}

// Edge: addModule with nullptr
TEST(CyclicTaskTest, AddModuleNullptr) {
    TestCyclicTask task(1000);
    task.addModule(nullptr);
    EXPECT_EQ(task.modules.size(), 1u);
    EXPECT_EQ(task.modules[0], nullptr);
    // cycleTask should not crash even if module is nullptr
    EXPECT_NO_THROW(task.cycleTask());
}

// Edge: interval is zero and corrected to minimum value of 10ms
TEST(CyclicTaskTest, ZeroIntervalSchedulesEveryTime) {
    TestCyclicTask task(0);
    task.initializeTaskTimer(100);
    EXPECT_FALSE(task.isRunScheduled(100));
    EXPECT_FALSE(task.isRunScheduled(101));
    EXPECT_TRUE(task.isRunScheduled(1000));
}

// Edge: very large interval, limited to maximum value of 10s
TEST(CyclicTaskTest, LargeInterval) {
    TestCyclicTask task(0xFFFFFFFFu);
    task.initializeTaskTimer(1);
    EXPECT_FALSE(task.isRunScheduled(100));
    EXPECT_FALSE(task.isRunScheduled(10000));
    EXPECT_TRUE(task.isRunScheduled(20000));
}

// Edge: interval clamping in constructor
TEST(CyclicTaskTest, IntervalClamping) {
    TestCyclicTask t1(1);      // Should clamp to 10
    EXPECT_EQ(t1.interval, 10u);
    TestCyclicTask t2(20000);  // Should clamp to 10000
    EXPECT_EQ(t2.interval, 10000u);
    TestCyclicTask t3(500);    // Should keep 500
    EXPECT_EQ(t3.interval, 500u);
}