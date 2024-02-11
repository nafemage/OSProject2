#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"

// Using a C library requires extern "C" to prevent function managling
extern "C" 
{
  #include <dyn_array.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

// Unit tests for Shortest Job First
TEST(shortest_job_first, StandardCase) {
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;

    // Create multiple PCB's with differing burst times
    for (int i = 0; i < 5; ++i) {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = (i + 1) * 2;
        pcb.priority = i + 1;
        pcb.arrival = i * 2;
        pcb.started = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success) {
            fprintf(stderr, "Failed to push ProcessControlBlock to dyn_array\n");
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm
    bool sjf_result = shortest_job_first(ready_queue, &result);
    EXPECT_TRUE(sjf_result);
    dyn_array_destroy(ready_queue);
}

TEST(shortest_job_first, ErrorChecking) {
    // Null Ready Queue Pointer
    dyn_array_t *null_ready_queue = NULL;
    ScheduleResult_t result;
    EXPECT_FALSE(shortest_job_first(null_ready_queue, &result));

    // Ready Queue of size 0
    dyn_array_t *zero_ready_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(shortest_job_first(zero_ready_queue, &result));
    dyn_array_destroy(zero_ready_queue);

    // Null Result Pointer
    dyn_array_t *valid_ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(shortest_job_first(valid_ready_queue, NULL));
    dyn_array_destroy(valid_ready_queue);
}

TEST(shortest_job_first, EqualBurstTime) {
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;

    // Create multiple PCB's with the same burst times
    for (int i = 0; i < 5; ++i) {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = 10;
        pcb.priority = i + 1;
        pcb.arrival = i * 2;
        pcb.started = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success) {
            fprintf(stderr, "Failed to push ProcessControlBlock to dyn_array\n");
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm
    bool sjf_result = shortest_job_first(ready_queue, &result);
    EXPECT_TRUE(sjf_result);
    dyn_array_destroy(ready_queue);
}

// TODO:
// Unit tests for Round Robin
TEST (round_robin, StandardCase) {
	EXPECT_NE('1','2');
}

TEST (round_robin, ErrorChecking) {
    EXPECT_NE('1','2');
}

TEST (round_robin, EqualBurstTime) {
    EXPECT_NE('1','2');
}

class GradeEnvironment : public testing::Environment 
{
    public:
        virtual void SetUp() 
        {
            score = 0;
            total = 210;
        }

        virtual void TearDown()
        {
            ::testing::Test::RecordProperty("points_given", score);
            ::testing::Test::RecordProperty("points_total", total);
            std::cout << "SCORE: " << score << '/' << total << std::endl;
        }
};


int main(int argc, char **argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new GradeEnvironment);
    return RUN_ALL_TESTS();
}
