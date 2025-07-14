#include "gtest/gtest.h"
#include "../../StateMachine.h"

// Helper to count callback invocations and capture arguments
struct CallbackMock {
    int count = 0;
    Status from = Status::idle;
    Status to = Status::idle;
    void operator()(Status f, Status t) {
        count++;
        from = f;
        to = t;
    }
};

TEST(HaySteamerStateMachineTest, InitialStatusIsIdle) {
    HaySteamerStateMachine sm;
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, ValidTransitionSequence) {
    HaySteamerStateMachine sm;
    CallbackMock cb1, cb2, cb3, cb4;
    sm.registerCallback(Status::idle, Status::ready, std::ref(cb1));
    sm.registerCallback(Status::ready, Status::heating, std::ref(cb2));
    sm.registerCallback(Status::heating, Status::holding, std::ref(cb3));
    sm.registerCallback(Status::holding, Status::done, std::ref(cb4));

    sm.changeStatus(Status::ready);
    EXPECT_EQ(sm.getCurrentStatus(), Status::ready);
    EXPECT_EQ(cb1.count, 1);
    EXPECT_EQ(cb1.from, Status::idle);
    EXPECT_EQ(cb1.to, Status::ready);

    sm.changeStatus(Status::heating);
    EXPECT_EQ(sm.getCurrentStatus(), Status::heating);
    EXPECT_EQ(cb2.count, 1);
    EXPECT_EQ(cb2.from, Status::ready);
    EXPECT_EQ(cb2.to, Status::heating);

    sm.changeStatus(Status::holding);
    EXPECT_EQ(sm.getCurrentStatus(), Status::holding);
    EXPECT_EQ(cb3.count, 1);
    EXPECT_EQ(cb3.from, Status::heating);
    EXPECT_EQ(cb3.to, Status::holding);

    sm.changeStatus(Status::done);
    EXPECT_EQ(sm.getCurrentStatus(), Status::done);
    EXPECT_EQ(cb4.count, 1);
    EXPECT_EQ(cb4.from, Status::holding);
    EXPECT_EQ(cb4.to, Status::done);
}

TEST(HaySteamerStateMachineTest, TransitionToErrorFromAnyState) {
    HaySteamerStateMachine sm;
    CallbackMock cb;
    sm.registerCallback(Status::idle, Status::error, std::ref(cb));
    sm.changeStatus(Status::error);
    EXPECT_EQ(sm.getCurrentStatus(), Status::error);
    EXPECT_EQ(cb.count, 1);
    EXPECT_EQ(cb.from, Status::idle);
    EXPECT_EQ(cb.to, Status::error);
}

TEST(HaySteamerStateMachineTest, ErrorToIdleIsAllowed) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::error);
    CallbackMock cb;
    sm.registerCallback(Status::error, Status::idle, std::ref(cb));
    sm.changeStatus(Status::idle);
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
    EXPECT_EQ(cb.count, 1);
    EXPECT_EQ(cb.from, Status::error);
    EXPECT_EQ(cb.to, Status::idle);
}

TEST(HaySteamerStateMachineTest, InvalidTransitionIsIgnored) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::ready);
    sm.changeStatus(Status::done); // Not allowed: ready -> done
    EXPECT_EQ(sm.getCurrentStatus(), Status::ready);
}

TEST(HaySteamerStateMachineTest, NoStatusChangeDoesNothing) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::idle);
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, RegisterCallbackOverridesPrevious) {
    HaySteamerStateMachine sm;
    CallbackMock cb1, cb2;
    sm.registerCallback(Status::idle, Status::ready, std::ref(cb1));
    sm.registerCallback(Status::idle, Status::ready, std::ref(cb2));
    sm.changeStatus(Status::ready);
    EXPECT_EQ(cb1.count, 0);
    EXPECT_EQ(cb2.count, 1);
}
