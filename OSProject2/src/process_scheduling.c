#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

// You might find this handy.  I put it around unused parameters, but you should
// remove it before you submit. Just allows things to compile initially.
#define UNUSED(x) (void)(x)

int compare_remaining_burst_time(const void *a, const void *b);

// Private function for decreasing the execution time of a process
void virtual_cpu(ProcessControlBlock_t *process_control_block, uint32_t execution_time) {
    // Decrement burst time of the PCB
    process_control_block->remaining_burst_time=process_control_block->remaining_burst_time-execution_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result) {
    // Invalid parameters
    if (ready_queue == NULL || result == NULL) return false;

    // No processes
    size_t num_processes = dyn_array_size(ready_queue);
    if (num_processes == 0) return false;

    // Sort the queue based on remaining burst time
    dyn_array_sort(ready_queue, compare_remaining_burst_time);

    // Initialize variables for accumulation and calculation
    unsigned int process_count = num_processes;
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    float execution_time = 0.0;

    // Iterate through the processes using a while loop
    size_t i = 0;
    while (num_processes > 0) {
        while (i < num_processes) {
            ProcessControlBlock_t *current_process = dyn_array_at(ready_queue, i);

            // Check if the process has arrived based on arrival time
            if (current_process->arrival <= execution_time) {
                // Calculate times for the scheduled process
                float waiting_time = execution_time - current_process->arrival;
                float burst_time = current_process->remaining_burst_time;

                // Call virtual_cpu to decrement the burst time
                virtual_cpu(current_process, burst_time);
                current_process->started = true;
                current_process->completed = true;

                // Update the schedule result
                total_waiting_time += waiting_time;
                total_turnaround_time += execution_time - current_process->arrival;
                execution_time += burst_time;

                // Remove the scheduled process from the ready_queue
                dyn_array_erase(ready_queue, i);
                num_processes--; // Decrement the number of processes after removal
            } else { // If the process hasn't arrived yet, move to the next process
                i++;
            }
        }
    }

    // Update the average waiting and turnaround times
    result->average_waiting_time = total_waiting_time / process_count;
    result->average_turnaround_time = total_turnaround_time / process_count;
    result->total_run_time = execution_time;

    return true;
}


bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    // Invalid parameters
    if (ready_queue == NULL || result == NULL || quantum <= 0) return false;

    // No processes
    size_t num_processes = dyn_array_size(ready_queue);
    if (num_processes == 0) return false;

    // Iterate through the processes
    for (size_t i = 0; i < num_processes; ++i) {
        ProcessControlBlock_t *current_process = dyn_array_at(ready_queue, i);

        // Check to see if the PCB has been completed
        if (current_process->completed != true) {
            // Calculate times for the scheduled process
            float waiting_time = result->total_run_time - current_process->arrival;
            float burst_time = current_process->remaining_burst_time;

            // Execute process in its entirety, or up to the burst time
            if (quantum <= burst_time) {
                // Call virtual_cpu to decrement the burst time
                virtual_cpu(current_process, burst_time);
                result->total_run_time += burst_time;
                current_process->completed = true;
            } else {
                // Call virtual_cpu to decrement the burst time, to the quantum
                virtual_cpu(current_process, quantum);
                result->total_run_time += quantum;
            }
            current_process->started = true;


            // Update the schedule result
            result->average_waiting_time =
                (result->average_waiting_time * i + waiting_time) / (i + 1);
            result->average_turnaround_time =
                (result->average_turnaround_time * i +
                result->total_run_time - current_process->arrival) / (i + 1);
            }
    }

    // Remove the scheduled process from the ready_queue
    dyn_array_erase(ready_queue, 0);
    return true;
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

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

// Function for sorting based on burst time
int compare_remaining_burst_time(const void *a, const void *b) {
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b;

    // Compare remaining burst times
    if (pcb_a->remaining_burst_time < pcb_b->remaining_burst_time) {
        return -1;
    } else if (pcb_a->remaining_burst_time > pcb_b->remaining_burst_time) {
        return 1;
    } else {
        return 0;
    }
}