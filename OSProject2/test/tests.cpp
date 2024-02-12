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

TEST(load_process_control_blocks, BadExistingFilename)
{
    EXPECT_EQ(nullptr, load_process_control_blocks("../pcb-bad-1.bin"));
}

TEST(load_process_control_blocks, GoodExistingFilename)
{
    dyn_array_t *array = load_process_control_blocks("../pcb.bin");
    EXPECT_NE(nullptr, array);
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

TEST(shortest_remaining_time_first, SuccessfulRun)
{
    uint32_t arrivals[] = {25, 30, 25};
    uint32_t priorities[] = {0, 1, 2};
    uint32_t remaining_burst_times[] = {100, 100, 5};
    bool started[] = {false, false, false};
    int count = 3;
    dyn_array_t *array = create_dyn_pcb_array(arrivals, priorities, remaining_burst_times, started, count);
    ScheduleResult_t *sr = (ScheduleResult_t *)malloc(sizeof(ScheduleResult_t));
    EXPECT_EQ(true, shortest_remaining_time_first(array, sr));
    print_pcb_array((ProcessControlBlock_t *)(array->array), array->size);
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
