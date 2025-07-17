#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../../HaySteamerLogic.h"

// Helper types for compatibility
using String = std::string;

// Mock functions for input
struct HaySteamerLogicMocks {
    MOCK_METHOD(bool, startConditions, (), (const));
    MOCK_METHOD(bool, startTimer, (), (const));
    MOCK_METHOD(bool, runTimer, (), (const));
    MOCK_METHOD(unsigned long, getTimeOfDayInMinutes, (), (const));
    MOCK_METHOD(int, getTemperature, (), (const));
    MOCK_METHOD(int, getMinimumTemperature, (), (const));
    MOCK_METHOD(unsigned long, getWaitTime, (), (const));
    MOCK_METHOD(String, hasFault, (Status), (const));
};

// Helper to set all input functions
void setAllInputs(HaySteamerLogic& logic, HaySteamerLogicMocks& mocks) {
    logic.setStartConditions([&] { return mocks.startConditions(); });
    logic.setStartTimer([&] { return mocks.startTimer(); });
    logic.setRunTimer([&] { return mocks.runTimer(); });
    logic.setGetTimeOfDayInMinutes([&] { return mocks.getTimeOfDayInMinutes(); });
    logic.setGetTemperature([&] { return mocks.getTemperature(); });
    logic.setGetMinimumTemperature([&] { return mocks.getMinimumTemperature(); });
    logic.setGetWaitTime([&] { return mocks.getWaitTime(); });
    logic.setHasFault([&](Status s) { return mocks.hasFault(s); });
}

// Test fixture
class HaySteamerLogicTest : public ::testing::Test {
protected:
    HaySteamerLogic logic;
    HaySteamerLogicMocks mocks;
    void SetUp() override {
        setAllInputs(logic, mocks);
    }
};

// Test: Initial status is idle, update with startConditions true triggers heating
TEST_F(HaySteamerLogicTest, IdleToHeatingTransition) {
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::heating);
    EXPECT_EQ(logic.getMessage(), "heating");
}

// Test: Idle, startConditions false, startTimer true triggers ready
TEST_F(HaySteamerLogicTest, IdleToReadyTransition) {
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, startTimer()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, hasFault(Status::ready)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::ready);
    EXPECT_EQ(logic.getMessage(), "ready");
}

// Test: Idle, startConditions false, startTimer false stays idle
TEST_F(HaySteamerLogicTest, IdleNoStartConditionOrTimerStaysIdle) {
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, startTimer()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, hasFault(Status::idle)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::idle);
    EXPECT_EQ(logic.getMessage(), "idle");
}

// Test: Ready, runTimer true triggers heating
TEST_F(HaySteamerLogicTest, ReadyToHeatingTransition) {
    // Move to ready first
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, startTimer()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, hasFault(Status::ready)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, ready: runTimer true
    EXPECT_CALL(mocks, runTimer()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(101));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(61));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::heating);
    EXPECT_EQ(logic.getMessage(), "heating");
}

// Test: Ready, runTimer false stays ready
TEST_F(HaySteamerLogicTest, ReadyNoRunTimerStaysReady) {
    // Move to ready first
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, startTimer()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, hasFault(Status::ready)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, ready: runTimer false
    EXPECT_CALL(mocks, runTimer()).WillOnce(::testing::Return(false));
    EXPECT_CALL(mocks, hasFault(Status::ready)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::ready);
}

// Test: Heating to Holding transition when temperature >= minimumTemperature
TEST_F(HaySteamerLogicTest, HeatingToHoldingTransition) {
    // Move to heating
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, heating: temperature >= minimumTemperature
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(60));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(200));
    EXPECT_CALL(mocks, getWaitTime()).WillOnce(::testing::Return(30));
    EXPECT_CALL(mocks, hasFault(Status::holding)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::holding);
    EXPECT_EQ(logic.getMessage(), "holding");
}

// Test: Heating, temperature < minimumTemperature, stays in heating
TEST_F(HaySteamerLogicTest, HeatingNoTempStaysHeating) {
    // Move to heating
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, heating: temperature < minimumTemperature
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(59));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::heating);
}

// Test: Holding to Done transition when wait time elapsed
TEST_F(HaySteamerLogicTest, HoldingToDoneTransition) {
    // Move to holding
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(60));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(200));
    EXPECT_CALL(mocks, getWaitTime()).WillOnce(::testing::Return(30));
    EXPECT_CALL(mocks, hasFault(Status::holding)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, holding: time elapsed
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(231)); // 200+31 >= 30
    EXPECT_CALL(mocks, hasFault(Status::done)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::done);
    EXPECT_EQ(logic.getMessage(), "done");
}

// Test: Done to Idle transition after 60 minutes
TEST_F(HaySteamerLogicTest, DoneToIdleTransition) {
    // Move to done
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(60));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(200));
    EXPECT_CALL(mocks, getWaitTime()).WillOnce(::testing::Return(30));
    EXPECT_CALL(mocks, hasFault(Status::holding)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(231));
    EXPECT_CALL(mocks, hasFault(Status::done)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, done: timeWhenDone = 231, time = 291 (231+60)
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(291));
    EXPECT_CALL(mocks, hasFault(Status::idle)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::idle);
    EXPECT_EQ(logic.getMessage(), "idle");
}

// Test: Heating timeout triggers error
TEST_F(HaySteamerLogicTest, HeatingTimeoutTriggersError) {
    // Move to heating
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, heating: timeOfDay - actualStartTime > heatingTimeout
    EXPECT_CALL(mocks, getTemperature()).WillOnce(::testing::Return(59));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(161)); // 100+61 > 60
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::error);
    EXPECT_EQ(logic.getMessage(), "heating timeout");
}

// Test: Holding temperature drop triggers error
TEST_F(HaySteamerLogicTest, HoldingTemperatureDropTriggersError) {
    // Move to holding
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(60));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(200));
    EXPECT_CALL(mocks, getWaitTime()).WillOnce(::testing::Return(30));
    EXPECT_CALL(mocks, hasFault(Status::holding)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, holding: temperature < minimumTemperature - holdingTemperatureDrop
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillOnce(::testing::Return(200));
    EXPECT_CALL(mocks, getTemperature()).WillOnce(::testing::Return(54)); // 60-5=55, so 54 triggers
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::error);
    EXPECT_EQ(logic.getMessage(), "temperature drop");
}

// Test: hasFault returns error message, triggers error
TEST_F(HaySteamerLogicTest, HasFaultTriggersError) {
    // Move to heating
    EXPECT_CALL(mocks, startConditions()).WillOnce(::testing::Return(true));
    EXPECT_CALL(mocks, getTimeOfDayInMinutes()).WillRepeatedly(::testing::Return(100));
    EXPECT_CALL(mocks, getMinimumTemperature()).WillOnce(::testing::Return(60));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return(""));
    logic.update();
    // Now, heating: no timeout, but hasFault returns error
    EXPECT_CALL(mocks, getTemperature()).WillRepeatedly(::testing::Return(59));
    EXPECT_CALL(mocks, hasFault(Status::heating)).WillOnce(::testing::Return("custom fault"));
    logic.update();
    EXPECT_EQ(logic.getCurrentStatus(), Status::error);
    EXPECT_EQ(logic.getMessage(), "custom fault");
}

// Test: set* functions ignore nullptr
TEST_F(HaySteamerLogicTest, SetFunctionsIgnoreNullptr) {
    HaySteamerLogic l;
    l.setStartConditions(nullptr);
    l.setStartTimer(nullptr);
    l.setRunTimer(nullptr);
    l.setGetTimeOfDayInMinutes(nullptr);
    l.setGetTemperature(nullptr);
    l.setGetMinimumTemperature(nullptr);
    l.setGetWaitTime(nullptr);
    l.setHasFault(nullptr);
    // Should not crash, but update will likely throw if called
    EXPECT_NO_THROW(l.getMessage());
    EXPECT_NO_THROW(l.getCurrentStatus());
}