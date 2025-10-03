#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../TaskScheduler.h"

// Helper: Expose internals for testing
struct TestOutputTask : public OutputTask {
    using OutputTask::OutputTask;
    using OutputTask::nextRun;
    using OutputTask::lastRun;
    using OutputTask::enabledFast;
    using OutputTask::slowInterval;
    using OutputTask::fastInterval;
};

// initializeTaskTimer sets nextRun and lastRun
TEST(OutputTaskTest, InitializeTaskTimerSetsNextRunAndLastRun) {
    TestOutputTask task(1000, 100);
    task.nextRun = 0;
    task.lastRun = 0;
    task.initializeTaskTimer(12345);
    EXPECT_EQ(task.nextRun, 12345u + 1000u);
    EXPECT_EQ(task.lastRun, 12345u);
}

// enableFast sets enabledFast and may adjust nextRun
TEST(OutputTaskTest, EnableFastSetsEnabledFastTrue) {
    TestOutputTask task(1000, 100);
    task.enabledFast = false;
    task.lastRun = 1000;
    task.fastInterval = 100;
    task.nextRun = 2000;
    // Should adjust nextRun if lastRun + fastInterval < nextRun
    task.enableFast();
    EXPECT_TRUE(task.enabledFast);
    EXPECT_EQ(task.nextRun, 1100u);

    // Should not adjust nextRun if lastRun + fastInterval >= nextRun
    task.nextRun = 1050;
    task.enableFast();
    EXPECT_TRUE(task.enabledFast);
    EXPECT_EQ(task.nextRun, 1050u);
}

// disableFast sets enabledFast false
TEST(OutputTaskTest, DisableFastSetsEnabledFastFalse) {
    TestOutputTask task(1000, 100);
    task.enabledFast = true;
    task.disableFast();
    EXPECT_FALSE(task.enabledFast);
}

// isRunScheduled returns false if not scheduled, true if scheduled, updates lastRun, and advances nextRun
TEST(OutputTaskTest, IsRunScheduledReturnsCorrectlyAndUpdatesLastRun) {
    TestOutputTask task(1000, 100);
    task.slowInterval = 1000;
    task.fastInterval = 100;
    task.enabledFast = false;
    task.nextRun = 2000;
    task.lastRun = 0;

    // Not scheduled yet
    EXPECT_FALSE(task.isRunScheduled(1999));
    EXPECT_EQ(task.lastRun, 0u);

    // Scheduled, slow mode
    EXPECT_TRUE(task.isRunScheduled(2000));
    EXPECT_EQ(task.lastRun, 2000u);
    EXPECT_EQ(task.nextRun, 3000u);

    // Scheduled, missed multiple intervals, slow mode
    task.nextRun = 4000;
    EXPECT_TRUE(task.isRunScheduled(6500));
    EXPECT_EQ(task.nextRun, 7000u);

    // Fast mode: nextRun advances by fastInterval
    task.enabledFast = true;
    task.nextRun = 8000;
    EXPECT_TRUE(task.isRunScheduled(8100));
    EXPECT_EQ(task.nextRun, 8200u);
}

// isRunScheduled switches between fast and slow intervals
TEST(OutputTaskTest, IsRunScheduledSwitchesIntervals) {
    TestOutputTask task(1000, 100);
    task.slowInterval = 1000;
    task.fastInterval = 100;
    task.enabledFast = false;
    task.nextRun = 1000;

    // Slow mode
    EXPECT_TRUE(task.isRunScheduled(1000));
    EXPECT_EQ(task.nextRun, 2000u);

    // Switch to fast mode
    task.enabledFast = true;
    task.nextRun = 2000;
    EXPECT_TRUE(task.isRunScheduled(2100));
    EXPECT_EQ(task.nextRun, 2200u);

    // Switch back to slow mode
    task.enabledFast = false;
    task.nextRun = 3000;
    EXPECT_TRUE(task.isRunScheduled(3000));
    EXPECT_EQ(task.nextRun, 4000u);
}

// Edge: isRunScheduled with multiple missed intervals in fast mode
TEST(OutputTaskTest, IsRunScheduledMultipleMissedFastIntervals) {
    TestOutputTask task(1000, 100);
    task.slowInterval = 1000;
    task.fastInterval = 100;
    task.enabledFast = true;
    task.nextRun = 1000;
    EXPECT_TRUE(task.isRunScheduled(1350));
    EXPECT_EQ(task.nextRun, 1400u);
}

// Edge: enableFast does not adjust nextRun if not needed
TEST(OutputTaskTest, EnableFastNoNextRunAdjustmentIfNotNeeded) {
    TestOutputTask task(1000, 100);
    task.lastRun = 1000;
    task.fastInterval = 100;
    task.nextRun = 1050; // lastRun + fastInterval == 1100, which is > nextRun
    task.enableFast();
    EXPECT_EQ(task.nextRun, 1050u);
}