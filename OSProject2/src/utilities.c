#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

void print_pcb_array(ProcessControlBlock_t *pcb_array, size_t count);
ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remainging_burst_times, bool *started, int count);
ProcessControlBlock_t *create_pcb(uint32_t arrival, uint32_t priority, uint32_t remainging_burst_time, bool started, ProcessControlBlock_t *ptr);

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

ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count)
{
    ProcessControlBlock_t *pcb_array = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t) * count);
    for (int i = 0; i < count; i++)
    {
        create_pcb(arrivals[i], priorities[i], remaining_burst_times[i], started[i], &(pcb_array[i]));
    }
    return pcb_array;
}

ProcessControlBlock_t *create_pcb(uint32_t arrival, uint32_t priority, uint32_t remaining_burst_time, bool started, ProcessControlBlock_t *ptr)
{
    if (ptr == NULL)
    {
        ptr = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t));
    }

    ptr->arrival = arrival;
    ptr->priority = priority;
    ptr->remaining_burst_time = remaining_burst_time;
    ptr->started = started;
    return ptr;
}