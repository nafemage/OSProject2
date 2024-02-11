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
    // dyn_array_t *dyn_array = (dyn_array_t *)malloc(sizeof(dyn_array_t));
    // dyn_array->array = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t));
    EXPECT_EQ(false, shortest_remaining_time_first(NULL, NULL));
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
