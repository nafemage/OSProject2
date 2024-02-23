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

int compare_remaining_burst_time(const void *a, const void *b);

// Private function for decreasing the execution time of a process
void virtual_cpu(ProcessControlBlock_t *process_control_block, uint32_t execution_time)
{
    // Decrement burst time of the PCB
    process_control_block->remaining_burst_time = process_control_block->remaining_burst_time - execution_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    UNUSED(ready_queue);
    UNUSED(result);
    return false;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0)
        return false;

    // Sort the ready queue based on remaining burst time (shortest job first)
    dyn_array_sort(ready_queue, compare_remaining_burst_time);

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);

    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(ready_queue) > 0)
    {
        // Find the shortest arrival time among all PCBs
        uint32_t shortest_arrival_time = UINT32_MAX;

        for (size_t i = 0; i < dyn_array_size(ready_queue); ++i)
        {
            const ProcessControlBlock_t *pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

            if (pcb->arrival < shortest_arrival_time)
            {
                shortest_arrival_time = pcb->arrival;
            }
        }

        // Move total_run_time forward by the shortest arrival time
        total_run_time = (shortest_arrival_time > total_run_time) ? shortest_arrival_time : total_run_time;

        // Get the PCB with the shortest remaining burst time
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);

        // Check if the PCB meets the correct standards
        if (pcb->arrival <= total_run_time && !pcb->started && !pcb->completed)
        {
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
        }
        else
        {
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
    if (readme_file == NULL)
    {
        fprintf(stderr, "Error opening ../readme.md for reading and writing\n");
        return false;
    }

    // Seek to the beginning of line 14
    fseek(readme_file, 0, SEEK_SET);
    for (int i = 0; i < 13; ++i)
        while (fgetc(readme_file) != '\n')
            ;

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

bool round_robin(dyn_array_t *ready_queue, ScheduleResult_t *result, size_t quantum)
{
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0 || quantum == 0)
        return false;

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);

    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(ready_queue) > 0)
    {
        // Find the shortest arrival time among all PCBs
        uint32_t shortest_arrival_time = UINT32_MAX;

        for (size_t i = 0; i < dyn_array_size(ready_queue); ++i)
        {
            const ProcessControlBlock_t *pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, i);

            if (pcb->arrival < shortest_arrival_time)
            {
                shortest_arrival_time = pcb->arrival;
            }
        }

        // Move total_run_time forward by the shortest arrival time
        total_run_time = (shortest_arrival_time > total_run_time) ? shortest_arrival_time : total_run_time;

        // Get the next pcb in the queue
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0);

        // Check if the PCB meets the correct standards
        if (pcb->arrival <= total_run_time && !pcb->completed)
        {
            // If PCB is being started for the first time
            if (!pcb->started)
                total_waiting_time += total_run_time - pcb->arrival;

            // Mark PCB as started
            pcb->started = true;

            // Depending on whether the process will be executed in its entirety...
            if (pcb->remaining_burst_time <= quantum)
            {
                // Update statistics
                total_turnaround_time += total_run_time - pcb->arrival + pcb->remaining_burst_time;
                total_run_time += pcb->remaining_burst_time;

                // Execute the process
                virtual_cpu(pcb, pcb->remaining_burst_time);

                // Mark PCB as completed
                pcb->completed = true;

                // Remove the processed PCB from the ready_queue
                dyn_array_erase(ready_queue, 0);
            }
            else
            {
                // Update statistics
                total_run_time += quantum;

                // Execute the process for the quantum amount
                virtual_cpu(pcb, quantum);

                // Move PCB to the end of the queue, but DONT erase it
                dyn_array_push_back(ready_queue, pcb);
                dyn_array_erase(ready_queue, 0);
            }
        }
        else
        {
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
    if (readme_file == NULL)
    {
        fprintf(stderr, "Error opening ../readme.md for reading and writing\n");
        return false;
    }

    // Seek to the beginning of line 14
    fseek(readme_file, 0, SEEK_SET);
    for (int i = 0; i < 13; ++i)
        while (fgetc(readme_file) != '\n')
            ;

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
        return NULL; // Return NULL if input_file is NULL (no file to read)
    }
    FILE *fp = fopen(input_file, "r"); // Open the file in read mode
    if (!fp)
    {
        return NULL; // Return NULL if the file wasn't able to be opened
    }
    uint32_t pcb_count;                                         // Variable for storing the pcb_count in the file (the first line in the file)
    size_t elements_read;                                       // Variable for storing the number of elements read when using fread
    elements_read = fread(&pcb_count, sizeof(uint32_t), 1, fp); // Read the first line and store the contents in pcb_content
    if (elements_read != 1)
    {
        return NULL; // Return NULL if less than 1 or more than 1 elements were read
    }
    ProcessControlBlock_t *pcb_array = malloc(sizeof(ProcessControlBlock_t) * pcb_count); // Allocate space for an array that can hold 'pcb_count' pcbs
    if (!pcb_array)
    {
        return NULL; // Return NULL if memory couldn't be allocated
    }
    for (uint32_t i = 0; i < pcb_count; i++) // Iterate through pcb_count
    {
        ProcessControlBlock_t *pcb = &(pcb_array[i]);                                 // Get the current pcb to read values for
        pcb->started = false;                                                         // Set the started value of the pcb to false
        elements_read = fread(&(pcb->remaining_burst_time), sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the burst time) and update the pcb's burst time
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
        elements_read = fread(&(pcb->priority), sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the priority) and update the pcb's priority
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
        elements_read = fread(&(pcb->arrival), sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the arrival) and update the pcb's arrival
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
    }
    // if (!feof(fp)) // Check if end of file (eof) hasn't been reached
    // {
    //     printf("Not end of file");
    //     fclose(fp);      // Close the file
    //     free(pcb_array); // Free pcb_array
    //     return NULL;     // Return NULL because the end of the file wasn't reached meaning the pcb_count was lower than the actual number of pcbs in the file
    // }
    fclose(fp);                                                                                           // Close the file
    dyn_array_t *dyn_array = dyn_array_import(pcb_array, pcb_count, sizeof(ProcessControlBlock_t), NULL); // Create a dyn_array out of the pcb_array
    free(pcb_array);                                                                                      // Free the pcb_array
    return dyn_array;                                                                                     // Return the dyn_array
}

// Sorts a and b by arrival time. If arrival time is equal then by burst time
int srtf_sort_arrival(const void *a, const void *b);

int srtf_sort_arrival(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a; // Cast the "a" variable to a pcb
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b; // Cast the "b" variable to a pcb
    if (pcb_a->arrival < pcb_b->arrival)
    {
        return -1; // pcb_a should be processed before pcb_b
    }
    else if (pcb_a->arrival > pcb_b->arrival)
    {
        return 1; // pcb_b should be processed before pcb_a
    }
    return pcb_a->remaining_burst_time - pcb_b->remaining_burst_time; // The pcb with the shortest burst time should be processed before the other
}

// Sorts a and b by burst time
int srtf_sort_burst(const void *a, const void *b);

int srtf_sort_burst(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a; // Cast the "a" variable to a pcb
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b; // Cast the "b" variable to a pcb
    return pcb_a->remaining_burst_time - pcb_b->remaining_burst_time;      // The pcb with the shortest burst time should be processed before the other
}

void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *));

void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *))
{
    if (ready_queue->size == 0)
    {
        return; // Return if ready_queue size is 0 (all processes have arrived)
    }
    const ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_front(ready_queue); // Get the first element in the ready_queue (the one with the smallest arrival time)

    if (current_processes->size == 0) // If no pcbs are in the current_processes queue
    {
        *current_wait_time = pcb->arrival; // Set the current_wait_time to the pcb's arrival ("fast forward")
    }
    while (ready_queue->size > 0 && pcb->arrival == *current_wait_time) // While pcbs are in the ready_queue and the pcb at the front of the queue has the same arrival time as the wait time
    {
        pcb = (ProcessControlBlock_t *)dyn_array_front(ready_queue); // Get the pcb at the front of the ready_queue
        // Note: storing dyn_array_front and following it by dyn_array_pop_front will change the value referenced by the dyn_array_front variable (this is why a create a new pcb with the same values)
        const ProcessControlBlock_t *pcb_cpy = create_pcb(pcb->arrival, pcb->priority, pcb->remaining_burst_time, pcb->started, NULL); // Copy the pcb at the front of the ready_queue
        dyn_array_pop_front(ready_queue);                                                                                              // Remove the pcb from the ready_queue
        dyn_array_insert_sorted(current_processes, pcb_cpy, cmp_fn);                                                                   // Insert the pcb into the current_processes queue (this will insert based on burst time)
    }
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false; // Return false if ready_queue is NULL (no pcbs to process) or result is NULL (no memory allocated for the result)
    }
    uint32_t process_count = ready_queue->size; // The number of processes in the queue
    float total_turnaround_time = 0;            // The sum of all turnaround times
    float total_wait_time = 0;                  // The sum of all wait times

    dyn_array_sort(ready_queue, srtf_sort_arrival); // sort array by arrival time (if equal then by burst time)

    dyn_array_t *current_processes = dyn_array_create(ready_queue->capacity, sizeof(ProcessControlBlock_t), NULL); // dyn_array fors holding the processes that have arrived
    uint32_t current_wait_time;                                                                                    // The time that has elapsed

    enqueue_processes(ready_queue, current_processes, &current_wait_time, srtf_sort_burst); // Add processes to the current_processes queue that have arrived and sort them by burst time

    while (current_processes->size) // While there are processes in the current_processes queue
    {
        ProcessControlBlock_t *pcb = ((ProcessControlBlock_t *)dyn_array_front(current_processes)); // Get the pcb at the front
        if (!pcb->started)
        {
            pcb->started = true; // Set the started property to true if it hasn't already been started
        }

        current_wait_time++;                // Increment the elapsed time
        virtual_cpu(pcb);                   // Send pcb to the cpu (decrement burst time)
        if (pcb->remaining_burst_time == 0) // If the pcb has finished
        {
            float turnaround_time = current_wait_time - pcb->arrival;   // The turnaround time for the pcb
            total_turnaround_time += turnaround_time;                   // Add to the total turnaround time
            total_wait_time += turnaround_time - pcb->total_burst_time; // Add to the total wait time
            dyn_array_pop_front(current_processes);                     // Remove the pcb from the queue
        }
        enqueue_processes(ready_queue, current_processes, &current_wait_time, srtf_sort_burst); // Add the processes to the current_processes queue that have arrived and sort them by burst time
    }
    dyn_array_destroy(current_processes);                                    // Free the current_processes array
    result->average_turnaround_time = total_turnaround_time / process_count; // Calculate and store the average turnaround time
    result->average_waiting_time = total_wait_time / process_count;          // Calculate and store the average wait time
    result->total_run_time = current_wait_time;                              // Store the total run time

    return true; // Return true because all processes were successfully completed
}

// Function for sorting based on burst time
int compare_remaining_burst_time(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a;
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b;

    // Compare remaining burst times
    if (pcb_a->remaining_burst_time < pcb_b->remaining_burst_time)
    {
        return -1;
    }
    else if (pcb_a->remaining_burst_time > pcb_b->remaining_burst_time)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}