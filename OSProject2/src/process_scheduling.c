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
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0) return false;

    // Sort the ready queue based on remaining burst time (shortest job first)
    dyn_array_sort(ready_queue, compare_remaining_burst_time);

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);

    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(ready_queue) > 0) {
        // Find the shortest arrival time among all PCBs
        uint32_t shortest_arrival_time = UINT32_MAX;

        for (size_t i = 0; i < dyn_array_size(ready_queue); ++i) {
            const ProcessControlBlock_t *pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

            if (pcb->arrival < shortest_arrival_time) {
                shortest_arrival_time = pcb->arrival;
            }
        }

        // Move total_run_time forward by the shortest arrival time
        total_run_time = (shortest_arrival_time > total_run_time) ? shortest_arrival_time : total_run_time;

        // Get the PCB with the shortest remaining burst time
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);

        // Check if the PCB meets the correct standards
        if (pcb->arrival <= total_run_time && !pcb->started && !pcb->completed) {
            // Mark PCB as started
            pcb->started = true;

            // Update statistics
            total_waiting_time += total_run_time - pcb->arrival;
            total_turnaround_time += total_run_time - pcb->arrival + pcb->remaining_burst_time;
            total_run_time += pcb->remaining_burst_time;
            
            // Execute the process
            virtual_cpu(pcb, pcb->remaining_burst_time);

            // Mark PCB as completed
            pcb->completed = true;

            // Remove the processed PCB from the ready_queue
            dyn_array_erase(ready_queue, 0);
        } else {
            // If the PCB doesn't meet the standards, move it to the end of the queue
            dyn_array_push_back(ready_queue, pcb);
            dyn_array_erase(ready_queue, 0);
        }
    }

    // Update the result structure with calculated averages
    result->average_waiting_time = total_waiting_time / starting_queue_size;
    result->average_turnaround_time = total_turnaround_time / starting_queue_size;
    result->total_run_time = total_run_time;

    // Open the readme.md file for both reading and writing
    FILE *readme_file = fopen("../readme.md", "r+");
    if (readme_file == NULL) {
        fprintf(stderr, "Error opening ../readme.md for reading and writing\n");
        return false;
    }

    // Seek to the beginning of line 14
    fseek(readme_file, 0, SEEK_SET);
    for (int i = 0; i < 13; ++i)
        while (fgetc(readme_file) != '\n');

    // Update lines 14, 15, and 16 directly in the file
    fprintf(readme_file, "Average Waiting Time: %f\n", result->average_waiting_time);
    fprintf(readme_file, "Average Turnaround Time: %f\n", result->average_turnaround_time);
    fprintf(readme_file, "Total Run Time: %lu\n", result->total_run_time);

    // Close the file
    fclose(readme_file);

    return true;
}


bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum) {
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0 || quantum == 0) return false;

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);

    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(ready_queue) > 0) {
        // Find the shortest arrival time among all PCBs
        uint32_t shortest_arrival_time = UINT32_MAX;

        for (size_t i = 0; i < dyn_array_size(ready_queue); ++i) {
            const ProcessControlBlock_t *pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

            if (pcb->arrival < shortest_arrival_time) {
                shortest_arrival_time = pcb->arrival;
            }
        }

        // Move total_run_time forward by the shortest arrival time
        total_run_time = (shortest_arrival_time > total_run_time) ? shortest_arrival_time : total_run_time;

        // Get the next pcb in the queue
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);

        // Check if the PCB meets the correct standards
        if (pcb->arrival <= total_run_time && !pcb->completed) {
            // If PCB is being started for the first time
            if (!pcb->started) total_waiting_time += total_run_time - pcb->arrival;

            // Mark PCB as started
            pcb->started = true;

            // Depending on whether the process will be executed in its entirety...
            if (pcb->remaining_burst_time <= quantum) {
                // Update statistics
                total_turnaround_time += total_run_time - pcb->arrival + pcb->remaining_burst_time;
                total_run_time += pcb->remaining_burst_time;

                // Execute the process
                virtual_cpu(pcb, pcb->remaining_burst_time);

                // Mark PCB as completed
                pcb->completed = true;

                // Remove the processed PCB from the ready_queue
                dyn_array_erase(ready_queue, 0);
            } else {
                // Update statistics
                total_run_time += quantum;

                // Execute the process for the quantum amount
                virtual_cpu(pcb, quantum);

                // Move PCB to the end of the queue, but DONT erase it
                dyn_array_push_back(ready_queue, pcb);
                dyn_array_erase(ready_queue, 0);
            }
        } else {
            // If the PCB doesn't meet the standards, move it to the end of the queue
            dyn_array_push_back(ready_queue, pcb);
            dyn_array_erase(ready_queue, 0);
        }
    }
    
    // Update the result structure with calculated averages
    result->average_waiting_time = total_waiting_time / starting_queue_size;
    result->average_turnaround_time = total_turnaround_time / starting_queue_size;
    result->total_run_time = total_run_time;

    // Open the readme.md file for both reading and writing
    FILE *readme_file = fopen("../readme.md", "r+");
    if (readme_file == NULL) {
        fprintf(stderr, "Error opening ../readme.md for reading and writing\n");
        return false;
    }

    // Seek to the beginning of line 14
    fseek(readme_file, 0, SEEK_SET);
    for (int i = 0; i < 13; ++i)
        while (fgetc(readme_file) != '\n');

    // Update lines 14, 15, and 16 directly in the file
    fprintf(readme_file, "Average Waiting Time: %f\n", result->average_waiting_time);
    fprintf(readme_file, "Average Turnaround Time: %f\n", result->average_turnaround_time);
    fprintf(readme_file, "Total Run Time: %lu\n", result->total_run_time);

    // Close the file
    fclose(readme_file);

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