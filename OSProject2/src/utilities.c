#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

void print_pcb_array(ProcessControlBlock_t *pcb_array, size_t count)
{
    for (size_t i = 0; i < count; i++)
    {
        ProcessControlBlock_t pcb = pcb_array[i];
        printf("PCB #%zu:\n", i + 1);
        printf("Remaining Burst Time: %u\n", pcb.remaining_burst_time);
        printf("Arrival Time: %u\n", pcb.arrival);
        printf("Priority: %u\n\n", pcb.priority);
    }
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
    ptr->total_burst_time = remaining_burst_time;
    ptr->completed = false;
    return ptr;
}

ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count)
{
    if (arrivals == NULL || priorities == NULL || remaining_burst_times == NULL || started == NULL)
    {
        return NULL;
    }
    ProcessControlBlock_t *pcb_array = (ProcessControlBlock_t *)malloc(sizeof(ProcessControlBlock_t) * count);
    for (int i = 0; i < count; i++)
    {
        create_pcb(arrivals[i], priorities[i], remaining_burst_times[i], started[i], &(pcb_array[i]));
    }
    return pcb_array;
}

dyn_array_t *create_dyn_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count)
{
    ProcessControlBlock_t *pcb_array = create_pcb_array(arrivals, priorities, remaining_burst_times, started, count);
    dyn_array_t *dyn_array = dyn_array_import(pcb_array, count, sizeof(ProcessControlBlock_t), NULL);
    free(pcb_array);
    return dyn_array;
}

void print_schedule_result(ScheduleResult_t *result)
{
    printf("Average waiting time: %f\n", result->average_waiting_time);
    printf("Average turnaround time: %f\n", result->average_turnaround_time);
    printf("Run time: %lu\n", result->total_run_time);
}

bool str_is_equal(char *str1, char *str2, int char_ct)
{
    return strncmp(str1, str2, char_ct) == 0;
}

bool is_fcfs(char *str)
{
    return str_is_equal(str, "fcfs", 5) || str_is_equal(str, "first_come_first_serve", 23);
}

bool is_sjf(char *str)
{
    return str_is_equal(str, "sjf", 4) || str_is_equal(str, "shortest_job_first", 19);
}

bool is_priority(char *str)
{
    return str_is_equal(str, "p", 2) || str_is_equal(str, "priority", 10);
}

bool is_rr(char *str)
{
    return str_is_equal(str, "rr", 3) || str_is_equal(str, "round_robin", 12);
}

bool is_srtf(char *str)
{
    return str_is_equal(str, "srtf", 5) || str_is_equal(str, "shortest_remaining_time_first", 30);
}