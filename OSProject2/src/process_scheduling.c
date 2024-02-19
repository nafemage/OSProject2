#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"
#include "utilities.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

// private function
void virtual_cpu(ProcessControlBlock_t *process_control_block)
{
    // decrement the burst time of the pcb
    --process_control_block->remaining_burst_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    UNUSED(ready_queue);
    UNUSED(result);
    UNUSED(quantum);
    return false;
}

dyn_array_t *load_process_control_blocks(const char *input_file)
{
    if (input_file == NULL)
    {
        return NULL;
    }
    FILE *fp = fopen(input_file, "r");
    if (!fp)
    {
        return NULL;
    }
    uint32_t pcb_count;
    size_t elements_read;
    elements_read = fread(&pcb_count, sizeof(uint32_t), 1, fp);
    if (elements_read != 1)
    {
        return NULL;
    }
    ProcessControlBlock_t *pcb_array = malloc(sizeof(ProcessControlBlock_t) * pcb_count);
    if (!pcb_array)
    {
        return NULL;
    }
    for (uint32_t i = 0; i < pcb_count; i++)
    {
        ProcessControlBlock_t *pcb = &(pcb_array[i]);
        pcb->started = false;
        elements_read = fread(&(pcb->remaining_burst_time), sizeof(uint32_t), 1, fp);
        if (elements_read != 1)
        {
            free(pcb_array);
            return NULL;
        }
        elements_read = fread(&(pcb->priority), sizeof(uint32_t), 1, fp);
        if (elements_read != 1)
        {
            free(pcb_array);
            return NULL;
        }
        elements_read = fread(&(pcb->arrival), sizeof(uint32_t), 1, fp);
        if (elements_read != 1)
        {
            free(pcb_array);
            return NULL;
        }
    }
    fclose(fp);
    dyn_array_t *dyn_array = dyn_array_import(pcb_array, pcb_count, sizeof(ProcessControlBlock_t), NULL);
    free(pcb_array);
    return dyn_array;
}

int srtf_sort_arrival(const void *a, const void *b);

int srtf_sort_arrival(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b;
    if (pcb_a->arrival < pcb_b->arrival)
    {
        return -1; // a comes before b
    }
    else if (pcb_a->arrival > pcb_b->arrival)
    {
        return 1; // b comes before a
    }
    return pcb_a->remaining_burst_time - pcb_b->remaining_burst_time;
}

int srtf_sort_compare_burst(const void *a, const void *b);

int srtf_sort_compare_burst(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b;
    return pcb_a->remaining_burst_time - pcb_b->remaining_burst_time;
}

void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *));

void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *))
{
    if (ready_queue->size == 0)
    {
        return;
    }
    const ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_front(ready_queue);

    if (current_processes->size == 0)
    {
        *current_wait_time = pcb->arrival;
    }
    while (ready_queue->size > 0)
    {
        // Note: storing dyn_array_front and following it by dyn_array_pop_front will change the value referenced by the dyn_array_front variable
        pcb = (ProcessControlBlock_t *)dyn_array_front(ready_queue);
        if (pcb->arrival == *current_wait_time)
        {
            const ProcessControlBlock_t *pcb_cpy = create_pcb(pcb->arrival, pcb->priority, pcb->remaining_burst_time, pcb->started, NULL);
            // printf("Enqueue 2: %u\n", pcb->arrival);
            dyn_array_pop_front(ready_queue);
            // printf("Enqueue 3: %u\n", pcb->arrival);
            dyn_array_insert_sorted(current_processes, pcb_cpy, cmp_fn);
        }
        else
        {
            return;
        }
    }
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false;
    }
    uint32_t process_count = ready_queue->size;
    float total_turnaround_time = 0;
    float total_wait_time = 0;

    dyn_array_sort(ready_queue, srtf_sort_arrival); // sort array by arrival time (if equal then by burst time)

    dyn_array_t *current_processes = dyn_array_create(ready_queue->capacity, sizeof(ProcessControlBlock_t), NULL);
    uint32_t current_wait_time;

    enqueue_processes(ready_queue, current_processes, &current_wait_time, srtf_sort_compare_burst);

    while (current_processes->size)
    {
        ProcessControlBlock_t *pcb = ((ProcessControlBlock_t *)dyn_array_front(current_processes));
        if (!pcb->started)
        {
            pcb->started = true;
        }

        current_wait_time++; // increment clock
        virtual_cpu(pcb);    // send to cpu, decrement burst time
        if (pcb->remaining_burst_time == 0)
        {
            float turnaround_time = current_wait_time - pcb->arrival;
            total_turnaround_time += turnaround_time;
            total_wait_time += turnaround_time - pcb->total_burst_time;
            dyn_array_pop_front(current_processes);
        }
        enqueue_processes(ready_queue, current_processes, &current_wait_time, srtf_sort_compare_burst); // check queue, if the process at the start is now available then sort again after popping to next waiting process (and setting arrival times to 0)
    }
    result->average_turnaround_time = total_turnaround_time / process_count;
    result->average_waiting_time = total_wait_time / process_count;
    result->total_run_time = current_wait_time;

    return true;
}