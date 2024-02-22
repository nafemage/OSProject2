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

TEST(load_process_control_blocks, FileLowCount) // Should low count be invalid?
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

// This is a test test message

#define NUM_PCB 30
#define QUANTUM 5 // Used for Robin Round for process as the run time limit

unsigned int score;
unsigned int total;

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
