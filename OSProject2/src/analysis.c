#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"
#include "utilities.h"

/*Moved to utitiles.c*/
// #define FCFS "FCFS"
// #define P "P"
// #define RR "RR"
// #define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        printf("Try passing in ../pcb.bin as the file name\n");
        return EXIT_FAILURE;
    }

    char *pcb_file = argv[1];
    char *algorithm = argv[2];

    dyn_array_t *ready_queue = load_process_control_blocks(pcb_file);
    ScheduleResult_t *sr = malloc(sizeof(ScheduleResult_t));

    bool algorithm_result = false;

    if (is_fcfs(algorithm))
    {
        algorithm_result = first_come_first_serve(ready_queue, sr);
    }
    else if (is_sjf(algorithm))
    {
        algorithm_result = shortest_job_first(ready_queue, sr);
    }
    else if (is_rr(algorithm))
    {
        if (argc < 4)
        {
            printf("Error: Please provide a quantum as the last parameter for round robin.\n");
            return EXIT_FAILURE;
        }
        size_t quantum;
        int res = sscanf(argv[3], "%zu", &quantum);
        if (res != 1)
        {
            printf("Error: Quantum was in an invalid format. Quantum received: %s.\n", argv[3]);
            return EXIT_FAILURE;
        }
        algorithm_result = round_robin(ready_queue, sr, quantum);
    }
    else if (is_srtf(algorithm))
    {
        algorithm_result = shortest_remaining_time_first(ready_queue, sr);
    }
    else
    {
        printf("Error: The schedule algorithm requested \'%s\' was not found.\n", algorithm);
        print_valid_algorithms();
        return EXIT_FAILURE;
    }
    // else if(is_priority(algorithm)){
    //     algorithm_result = priority(ready_queue, sr);
    // }

    if (algorithm_result)
    {
        print_schedule_result(sr);
    }
    else
    {
        printf("There was an error running the %s algorithm.\n", algorithm);
        return EXIT_FAILURE;
    }
    free(sr);
    dyn_array_destroy(ready_queue);

    return EXIT_SUCCESS;
}
