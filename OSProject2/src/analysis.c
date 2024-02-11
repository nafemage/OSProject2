#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

void print_dyn_pcb_array(dyn_array_t *a);

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        printf("Try passing in ../pcb.bin as the file name");
        return EXIT_FAILURE;
    }

    dyn_array_t *a = load_process_control_blocks(argv[1]);
    print_dyn_pcb_array(a->array, a->size);

    return EXIT_SUCCESS;
}

// testing purposes only
void print_pcb_array(ProcessControlBlock_t *pcb_array, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ProcessControlBlock_t pcb = pcb_array[i];
        printf("PCB #%zu:\n", i);
        printf("Remaining Burst Time: %u\n", pcb.remaining_burst_time);
        printf("Arrival Time: %u\n", pcb.arrival);
        printf("Priority: %u\n\n", pcb.priority);
    }
}
