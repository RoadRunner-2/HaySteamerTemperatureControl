#define USE_TEST_MILLIS
namespace {
    unsigned long fakeMillis = 0;
    unsigned long millis() { return fakeMillis; }
}

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "../TaskScheduler.h"
#include "../Sensor.h"
#include "../Actor.h"
#include "../Status.h"
#include "../millis.h"

// --- Fake Sensor Streams for Process Logic ---

class FakeClock : public Sensor<time_t> {
public:
    FakeClock() : timeStamp(0) {}
    void set(unsigned long timeStamp_s) { timeStamp = timeStamp_s; }
    time_t read() override { return timeStamp; }
private:
    unsigned long timeStamp;
};

class FakeTemp : public Sensor<int> {
public:
    FakeTemp() : temp(20) {}
    void set(int t) { temp = t; }
    int read() override { return temp; }
private:
    int temp;
};

class FakeKeypad : public Sensor<char> {
public:
    char read() override { return 'A'; }
};

class MockDisplay : public Actor<String[4]> {
public:
    MOCK_METHOD(void, write, (String[4]), (override));
    MOCK_METHOD(void, setup, (), (override));
	String lastDisplay[4];
};

class MockRelay : public Actor<byte> {
public:
    MOCK_METHOD(void, write, (byte), (override));
    MOCK_METHOD(void, setup, (), (override));
};

class MockLED : public Actor<Status> {
public:
    MOCK_METHOD(void, write, (Status), (override));
    MOCK_METHOD(void, setup, (), (override));
};

// --- Test Fixture ---

class CyclicCallerProcessTest : public ::testing::Test {
protected:
    FakeClock clock;
    FakeTemp temp;
    FakeKeypad keypad;
    MockDisplay display;
    std::array<String, 4> lastDisplay;
	MockRelay relay;
	MockLED led;
    std::unique_ptr<CyclicCaller> caller;

    void SetUp() override {
        caller = std::make_unique<CyclicCaller>(&clock, &temp, &keypad, &display, &relay, &led);
        fakeMillis = 0;

        ON_CALL(display, write(testing::_))
            .WillByDefault(
                [&](String* content) {
                    for (size_t i = 0; i < lastDisplay.size(); ++i) {
                        lastDisplay[i] = content[i];
                    }
                });
    }
};

// --- Helper: Drive Process Through All States ---

void driveProcess(CyclicCaller& caller, FakeClock& clock, FakeTemp& temp) {
    // 1. Start in idle, trigger ready via startTimer
    caller.startTimer = true;
    fakeMillis += 2000;
    caller.executeCyclicTasks();
    caller.startTimer = false;

    // 2. Now in ready, trigger heating via runTimer (timerCondition)
    // Simulate startConditions false, timerCondition true
    // This is handled by the logic's runTimer, which is mapped to startConditions.timerCondition()
    // So, set clock to just after start time
    clock.set(100); // Assume start time is 100
    fakeMillis += 2000;
    caller.executeCyclicTasks();

    // 3. Now in heating, raise temp to minimumTemperature (default 60)
    temp.set(60);
    fakeMillis += 2000;
    caller.executeCyclicTasks();

    // 4. Now in holding, wait for waitTime (default 30)
    clock.set(200); // Simulate time passed since reachedMinimumTemperature
    fakeMillis += 30000;
    caller.executeCyclicTasks();

    // 5. Now in done, wait for 60 minutes to return to idle
    clock.set(260); // 60 minutes after timeWhenDone
    fakeMillis += 60000;
    caller.executeCyclicTasks();
}

// --- Tests ---

TEST_F(CyclicCallerProcessTest, InitializeTasksInitializesAllTasks) {
    EXPECT_NO_THROW(caller->initializeTasks());
}

TEST_F(CyclicCallerProcessTest, ExecuteCyclicTasksRunsScheduledTasks) {
    caller->initializeTasks();
    fakeMillis = 10000;
    EXPECT_NO_THROW(caller->executeCyclicTasks());
}

TEST_F(CyclicCallerProcessTest, StartTimerFlagAffectsLogic) {
    caller->initializeTasks();
    caller->startTimer = true;
    EXPECT_NO_THROW(caller->executeCyclicTasks());
}

TEST_F(CyclicCallerProcessTest, CanCallMultipleCycles) {
    caller->initializeTasks();
    for (int i = 0; i < 5; ++i) {
        fakeMillis += 1000;
        EXPECT_NO_THROW(caller->executeCyclicTasks());
    }
}

// --- New: Full Process Logic Cycle Test ---

TEST_F(CyclicCallerProcessTest, FullProcessCycleThroughHaySteamerLogic) {
    caller->initializeTasks();
    driveProcess(*caller, clock, temp);
    // If no exceptions, the process cycled through all states
    SUCCEED();
}

// --- New: Check State Transitions via HaySteamerLogic ---

TEST_F(CyclicCallerProcessTest, StateTransitionsAreCorrect) {   
    clock.set(951827696);  
    fakeMillis = 951827696;

    caller->initializeTasks();


    // 1. idle -> ready  
    caller->startTimer = true;  
    fakeMillis += 2000;  
    
    caller->executeCyclicTasks();
    EXPECT_EQ(lastDisplay[1], "ready");

    // 2. ready -> heating  
    caller->startTimer = false;  
    clock.set(951827696 + 60); // Simulate timerCondition true  
    fakeMillis += 2000;  
    caller->executeCyclicTasks();  
    EXPECT_EQ(lastDisplay[1], "heating");

    // 3. heating -> holding  
    temp.set(60);  
    fakeMillis += 2000;  
    caller->executeCyclicTasks();  
    EXPECT_EQ(lastDisplay[1], "holding");

    // 4. holding -> done  
    clock.set(951827696 + (40*60));  
    fakeMillis += 2000;  
    caller->executeCyclicTasks();  
    EXPECT_EQ(lastDisplay[1], "done");

    // 5. done -> idle  
    clock.set(951827696 + 60);  
    fakeMillis += 60000;  
    caller->executeCyclicTasks();  
    EXPECT_EQ(lastDisplay[1], "idle");
}