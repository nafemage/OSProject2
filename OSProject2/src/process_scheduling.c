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