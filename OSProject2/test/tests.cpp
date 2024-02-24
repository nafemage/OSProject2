#include <fcntl.h>
#include <stdio.h>
#include "gtest/gtest.h"
#include <pthread.h>
#include "../include/processing_scheduling.h"

#include "utilities.h"

// Using a C library requires extern "C" to prevent function managling
extern "C"
{
#include <dyn_array.h>
}

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;


/*
 * Load PCB
 */
TEST(load_process_control_blocks, NullFilename)
{
    EXPECT_EQ(nullptr, load_process_control_blocks(NULL));
}

TEST(load_process_control_blocks, InvalidFilename)
{
    EXPECT_EQ(nullptr, load_process_control_blocks("\n"));
    EXPECT_EQ(nullptr, load_process_control_blocks("\0"));
}

TEST(load_process_control_blocks, NonExistingFilename)
{
    EXPECT_EQ(nullptr, load_process_control_blocks("test.bin"));
    EXPECT_EQ(nullptr, load_process_control_blocks("../test.bin"));
}

TEST(load_process_control_blocks, BadFileCountOnly)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/count-only.bin");
    EXPECT_EQ(nullptr, array);
}

TEST(load_process_control_blocks, BadFileNoArrival)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/no-arrival.bin");
    EXPECT_EQ(nullptr, array);
}

TEST(load_process_control_blocks, BadFileNoPriority)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/no-priority.bin");
    EXPECT_EQ(nullptr, array);
}

TEST(load_process_control_blocks, FileLowCount)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/low-count.bin");
    EXPECT_NE(nullptr, array);
    dyn_array_destroy(array);
}

TEST(load_process_control_blocks, BadFileHighCount)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/high-count.bin");
    EXPECT_EQ(nullptr, array);
}

TEST(load_process_control_blocks, GoodExistingFilename)
{
    dyn_array_t *array = load_process_control_blocks("../pcb.bin");
    EXPECT_NE(nullptr, array);
    dyn_array_destroy(array);
}

TEST(load_process_control_blocks, GoodFile1)
{
    dyn_array_t *array = load_process_control_blocks("../pcb_file_tests/files/zeroed-pcbs.bin");
    EXPECT_NE(nullptr, array);
    EXPECT_EQ((uint32_t)2, array->size);
    ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(array, 0);
    EXPECT_EQ((uint32_t)0, pcb->arrival);
    EXPECT_EQ((uint32_t)0, pcb->remaining_burst_time);
    EXPECT_EQ((uint32_t)0, pcb->priority);
    pcb = (ProcessControlBlock_t *)dyn_array_at(array, 1);
    EXPECT_EQ((uint32_t)0, pcb->arrival);
    EXPECT_EQ((uint32_t)0, pcb->remaining_burst_time);
    EXPECT_EQ((uint32_t)0, pcb->priority);
    dyn_array_destroy(array);
}

/*
 * Shortest Remaining Time First
 */
TEST(shortest_remaining_time_first, NullQueue)
{
    ScheduleResult_t *sr = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    EXPECT_EQ(false, shortest_remaining_time_first(NULL, sr));
    free(sr);
}

TEST(shortest_remaining_time_first, NullScheduleResult)
{
    uint32_t arrivals[] = {25};
    uint32_t priorities[] = {0};
    uint32_t remaining_burst_times[] = {100};
    bool started[] = {false};
    int count = 1;
    dyn_array_t *array = create_dyn_pcb_array(arrivals, priorities, remaining_burst_times, started, count);
    EXPECT_EQ(false, shortest_remaining_time_first(array, NULL));
    dyn_array_destroy(array);
}

// PCB #1:
//  Remaining Burst Time: 15
//  Arrival Time: 0
//  Priority: 0

// PCB #2:
// Remaining Burst Time: 10
// Arrival Time: 1
// Priority: 0

// PCB #3:
// Remaining Burst Time: 5
// Arrival Time: 2
// Priority: 0

// PCB #4:
// Remaining Burst Time: 20
// Arrival Time: 3
// Priority: 0

TEST(shortest_remaining_time_first, SuccessfulRunFile)
{
    dyn_array_t *array = load_process_control_blocks("../pcb.bin");
    ScheduleResult_t *sr = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    EXPECT_EQ(true, shortest_remaining_time_first(array, sr));
    EXPECT_EQ((uint32_t)0, array->size);
    EXPECT_NEAR((float)11.75, sr->average_waiting_time, .01);
    EXPECT_NEAR((float)24.25, sr->average_turnaround_time, .01);
    EXPECT_EQ((unsigned long)50, sr->total_run_time);
    free(sr);
    dyn_array_destroy(array);
}

TEST(shortest_remaining_time_first, SuccessfulRun1)
{
    uint32_t arrivals[] = {30, 25, 24};
    uint32_t priorities[] = {0, 1, 2};
    uint32_t remaining_burst_times[] = {100, 106, 5};
    bool started[] = {false, false, false};
    int count = 3;
    dyn_array_t *array = create_dyn_pcb_array(arrivals, priorities, remaining_burst_times, started, count);
    ScheduleResult_t *sr = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    EXPECT_EQ(true, shortest_remaining_time_first(array, sr));
    EXPECT_EQ((uint32_t)0, array->size);
    EXPECT_NEAR((float)34.67, sr->average_waiting_time, .01);
    EXPECT_NEAR((float)105.0, sr->average_turnaround_time, .01);
    EXPECT_EQ((unsigned long)235, sr->total_run_time);
    free(sr);
    dyn_array_destroy(array);
}

TEST(shortest_remaining_time_first, SuccessfulRun2)
{
    uint32_t arrivals[] = {0, 2, 4, 1, 1, 1};
    uint32_t priorities[] = {0, 0, 0, 0, 0, 0};
    uint32_t remaining_burst_times[] = {100, 96, 93, 99, 98, 98};
    bool started[] = {false, false, false, false, false, false};
    int count = 6;
    dyn_array_t *array = create_dyn_pcb_array(arrivals, priorities, remaining_burst_times, started, count);
    ScheduleResult_t *sr = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    EXPECT_EQ(true, shortest_remaining_time_first(array, sr));
    EXPECT_EQ((uint32_t)0, array->size);
    EXPECT_NEAR((float)239.67, sr->average_waiting_time, .01);
    EXPECT_NEAR((float)337, sr->average_turnaround_time, .01);
    EXPECT_EQ((unsigned long)584, sr->total_run_time);
    free(sr);
    dyn_array_destroy(array);
}



// Tests for First Come First Serve
TEST(first_come_first_serve, ErrorChecking) {
    // Null Ready Queue Pointer
    dyn_array_t *null_ready_queue = NULL;
    ScheduleResult_t result;
    EXPECT_FALSE(first_come_first_serve(null_ready_queue, &result));

    // Ready Queue of size 0
    dyn_array_t *zero_ready_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(first_come_first_serve(zero_ready_queue, &result));
    dyn_array_destroy(zero_ready_queue);

    // Null Result Pointer
    dyn_array_t *valid_ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(first_come_first_serve(valid_ready_queue, NULL));
    dyn_array_destroy(valid_ready_queue);
}

TEST(first_come_first_serve, StandardCase) {
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;
    result.average_waiting_time = 0.0;
    result.average_turnaround_time = 0.0;
    result.total_run_time = 0.0;

    // Create multiple PCB's with differing burst times
    for (int i = 0; i < 5; ++i) {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = (i + 1) * 2;
        pcb.priority = i + 1;
        pcb.arrival = i * 2;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success) {
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the FCFS scheduling algorithm and check results
    bool fcfs_result = first_come_first_serve(ready_queue, &result);
    EXPECT_TRUE(fcfs_result);
    dyn_array_destroy(ready_queue);
}


TEST(shortest_job_first, ErrorChecking)
{
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

TEST(shortest_job_first, StandardCase)
{
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;
    result.average_waiting_time = 0.0;
    result.average_turnaround_time = 0.0;
    result.total_run_time = 0.0;

    // Create multiple PCB's with differing burst times
    for (int i = 0; i < 5; ++i)
    {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = (i + 1) * 2;
        pcb.priority = i + 1;
        pcb.arrival = 0;
        pcb.started = false;
        pcb.completed = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success)
        {
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm and check results
    bool sjf_result = shortest_job_first(ready_queue, &result);
    EXPECT_TRUE(sjf_result);
    EXPECT_EQ(result.average_waiting_time, 8);
    EXPECT_EQ(result.average_turnaround_time, 14);
    EXPECT_EQ(result.total_run_time, (unsigned long)30);
    dyn_array_destroy(ready_queue);
}

TEST(shortest_job_first, EqualBurstTime)
{
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;

    // Create multiple PCB's with the same burst times
    for (int i = 0; i < 5; ++i)
    {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = 10;
        pcb.priority = i + 1;
        pcb.arrival = i * 20;
        pcb.started = false;
        pcb.completed = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success)
        {
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm
    bool sjf_result = shortest_job_first(ready_queue, &result);
    EXPECT_TRUE(sjf_result);
    EXPECT_EQ(result.average_waiting_time, 0);
    EXPECT_EQ(result.average_turnaround_time, 10);
    EXPECT_EQ(result.total_run_time, (unsigned long)90);
    dyn_array_destroy(ready_queue);
}

// Unit tests for Round Robin
TEST(round_robin, ErrorChecking)
{
    // Null Ready Queue Pointer
    dyn_array_t *null_ready_queue = NULL;
    ScheduleResult_t result;
    EXPECT_FALSE(round_robin(null_ready_queue, &result, 1));

    // Ready Queue of size 0
    dyn_array_t *zero_ready_queue = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(round_robin(zero_ready_queue, &result, 1));
    dyn_array_destroy(zero_ready_queue);

    // Null Result Pointer
    dyn_array_t *valid_ready_queue = dyn_array_create(1, sizeof(ProcessControlBlock_t), NULL);
    EXPECT_FALSE(round_robin(valid_ready_queue, NULL, 1));

    // Invalid Quantum
    EXPECT_FALSE(round_robin(valid_ready_queue, NULL, 0));
    EXPECT_FALSE(round_robin(valid_ready_queue, NULL, -1));
    dyn_array_destroy(valid_ready_queue);
}

TEST(round_robin, StandardCase)
{
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;
    size_t quantum = 1;

    // Create multiple PCB's with differing burst times
    for (int i = 0; i < 5; ++i)
    {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = 10;
        pcb.priority = i + 1;
        pcb.arrival = i * 5;
        pcb.started = false;
        pcb.completed = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success)
        {
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm
    bool round_robin_result = round_robin(ready_queue, &result, quantum);
    EXPECT_TRUE(round_robin_result);
    EXPECT_EQ(result.average_waiting_time, (float)0.8);
    EXPECT_EQ(result.average_turnaround_time, (float)29.2);
    EXPECT_EQ(result.total_run_time, (unsigned long)50);
    dyn_array_destroy(ready_queue);
}

TEST(round_robin, EqualBurstTime)
{
    // Create a dyn_array to hold ProcessControlBlock structures
    dyn_array_t *ready_queue = dyn_array_create(5, sizeof(ProcessControlBlock_t), NULL);
    ScheduleResult_t result;
    size_t quantum = 1;

    // Create multiple PCB's with differing burst times
    for (int i = 0; i < 5; ++i)
    {
        ProcessControlBlock_t pcb;
        pcb.remaining_burst_time = (i + 1) * 2;
        pcb.priority = i + 1;
        pcb.arrival = 0;
        pcb.started = false;
        pcb.completed = false;

        // Add the ProcessControlBlock to the dyn_array
        bool success = dyn_array_push_back(ready_queue, &pcb);
        if (!success)
        {
            dyn_array_destroy(ready_queue);
            FAIL();
        }
    }

    // Run the SJF scheduling algorithm
    bool round_robin_result = round_robin(ready_queue, &result, quantum);
    EXPECT_TRUE(round_robin_result);
    EXPECT_EQ(result.average_waiting_time, 2);
    EXPECT_EQ(result.average_turnaround_time, 20);
    EXPECT_EQ(result.total_run_time, (unsigned long)30);
    dyn_array_destroy(ready_queue);
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
