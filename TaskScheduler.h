#ifndef TaskScheduler_h
#define TaskScheduler_h

struct CyclicTask {
    void (*taskFunction)();   // Function to call
    unsigned long interval;   // Desired interval
    unsigned long lastRun;    // Timestamp of last execution
};

// Example functions to call cyclically
void task1() {
    Serial.println("Task 1 executed!");
}

void task2() {
    Serial.println("Task 2 executed!");
}

void task3() {
    Serial.println("Task 3 executed!");
}

// Array of tasks
CyclicTask tasks[] = {
    {task1, 1000, 0},  // Executes every 1000ms
    {task2, 2000, 0},  // Executes every 2000ms
    {task3, 3000, 0}   // Executes every 3000ms
};

const int taskCount = sizeof(tasks) / sizeof(CyclicTask);

// Function to execute cyclic tasks with adaptive timing
void executeCyclicTasks() {
    unsigned long currentMillis = millis();

    for (int i = 0; i < taskCount; i++) {
        if (currentMillis - tasks[i].lastRun >= tasks[i].interval) {
            tasks[i].lastRun = currentMillis;
            tasks[i].taskFunction();
        }
    }
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    executeCyclicTasks();
}

#endif
