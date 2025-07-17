#include "gtest/gtest.h"
#include "../../StateMachine.h"

TEST(HaySteamerStateMachineTest, InitialStatusIsIdle) {
    HaySteamerStateMachine sm;
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, ValidTransitionSequence) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::ready);
    EXPECT_EQ(sm.getCurrentStatus(), Status::ready);

    sm.changeStatus(Status::heating);
    EXPECT_EQ(sm.getCurrentStatus(), Status::heating);

    sm.changeStatus(Status::holding);
    EXPECT_EQ(sm.getCurrentStatus(), Status::holding);

    sm.changeStatus(Status::done);
    EXPECT_EQ(sm.getCurrentStatus(), Status::done);
}

TEST(HaySteamerStateMachineTest, TransitionToErrorFromAnyState) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::error);
    EXPECT_EQ(sm.getCurrentStatus(), Status::error);

    sm.changeStatus(Status::idle);
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, ErrorToIdleIsAllowed) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::error);
    EXPECT_EQ(sm.getCurrentStatus(), Status::error);

    sm.changeStatus(Status::idle);
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, InvalidTransitionLeadsToError) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::ready);
    sm.changeStatus(Status::done); // Not allowed: ready -> done
    EXPECT_EQ(sm.getCurrentStatus(), Status::error);

    sm.changeStatus(Status::ready); // Not allowed: error -> ready
    EXPECT_EQ(sm.getCurrentStatus(), Status::error);

    sm.changeStatus(Status::idle); // allowed: error -> idle
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}

TEST(HaySteamerStateMachineTest, NoStatusChangeDoesNothing) {
    HaySteamerStateMachine sm;
    sm.changeStatus(Status::idle);
    EXPECT_EQ(sm.getCurrentStatus(), Status::idle);
}
