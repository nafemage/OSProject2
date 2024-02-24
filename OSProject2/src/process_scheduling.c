#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "dyn_array.h"
#include "processing_scheduling.h"
#include "utilities.h"

// Private function for decreasing the execution time of a process
void virtual_cpu(ProcessControlBlock_t *process_control_block, uint32_t execution_time)
{
    // Decrement burst time of the PCB
    process_control_block->remaining_burst_time = process_control_block->remaining_burst_time - execution_time;
}

bool first_come_first_serve(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    // Invalid Params
    if(ready_queue == NULL || result == NULL) return false;
    
    //Number of processes in the ready_queue
    size_t num_processes = dyn_array_size(ready_queue);

    //Sort based on arrival (assuming processes can be in any order in the ready_queue)
    dyn_array_sort(ready_queue, compare_arrival);

    // No processes
    if(num_processes == 0) return false;

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);
    
    // Iterate through the processes
    for(size_t i = 0; i < num_processes; ++i){
        ProcessControlBlock_t *pcb = dyn_array_at(ready_queue, i);

        // Mark PCB as started
        pcb->started = true;

        // Update statistics
        if(total_run_time < pcb->arrival) {
            total_run_time = pcb->arrival;
        }
        total_run_time += pcb->remaining_burst_time;
        float turnaround_time = total_run_time - pcb->arrival;
        total_turnaround_time += turnaround_time;
        total_waiting_time += turnaround_time - pcb->remaining_burst_time;

        // Execute the process
        virtual_cpu(pcb, pcb->remaining_burst_time);

        // Mark PCB as completed
        pcb->completed = true;
    }
    
    // Update the result structure with calculated averages
    write_schedule_result(result, total_turnaround_time, total_waiting_time, total_run_time, starting_queue_size);

    return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0)
        return false;

    // Sort the ready queue based on remaining burst time (shortest job first)
    dyn_array_sort(ready_queue, compare_burst);

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
            total_run_time += pcb->remaining_burst_time;
            float turnaround_time = total_run_time - pcb->arrival;
            total_turnaround_time += turnaround_time;
            total_waiting_time += turnaround_time - pcb->remaining_burst_time;
            
            // total_waiting_time += total_run_time - pcb->arrival;
            // total_turnaround_time += total_run_time - pcb->arrival + pcb->remaining_burst_time;
            // total_run_time += pcb->remaining_burst_time;

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
    write_schedule_result(result, total_turnaround_time, total_waiting_time, total_run_time, starting_queue_size);

    return true;
}

// bool priority(dyn_array_t *ready_queue, ScheduleResult_t *result)
// {
//     UNUSED(ready_queue);
//     UNUSED(result);
//     return false;
// }

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
    write_schedule_result(result, total_turnaround_time, total_waiting_time, total_run_time, starting_queue_size);

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
    fclose(fp);                                                                                           // Close the file
    dyn_array_t *dyn_array = dyn_array_import(pcb_array, pcb_count, sizeof(ProcessControlBlock_t), NULL); // Create a dyn_array out of the pcb_array
    free(pcb_array);                                                                                      // Free the pcb_array
    return dyn_array;                                                                                     // Return the dyn_array
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || result == NULL)
    {
        return false; // Return false if ready_queue is NULL (no pcbs to process) or result is NULL (no memory allocated for the result)
    }
    uint32_t process_count = ready_queue->size; // The number of processes in the queue
    uint32_t total_turnaround_time = 0;         // The sum of all turnaround times
    uint32_t total_wait_time = 0;               // The sum of all wait times

    dyn_array_sort(ready_queue, compare_arrival_burst); // sort array by arrival time (if equal then by burst time)

    dyn_array_t *arrived_processes = dyn_array_create(ready_queue->capacity, sizeof(ProcessControlBlock_t), NULL); // dyn_array fors holding the processes that have arrived
    uint32_t current_wait_time;                                                                                    // The time that has elapsed

    enqueue_processes(ready_queue, arrived_processes, &current_wait_time, compare_burst); // Add processes to the arrived_processes queue that have arrived and sort them by burst time

    while (arrived_processes->size) // While there are processes in the arrived_processes queue
    {
        ProcessControlBlock_t *pcb = ((ProcessControlBlock_t *)dyn_array_front(arrived_processes)); // Get the pcb at the front
        if (!pcb->started)
        {
            pcb->started = true; // Set the started property to true if it hasn't already been started
        }

        current_wait_time++;                // Increment the elapsed time
        virtual_cpu(pcb, 1);                // Send pcb to the cpu (decrement burst time)
        if (pcb->remaining_burst_time == 0) // If the pcb has finished
        {
            uint32_t turnaround_time = current_wait_time - pcb->arrival; // The turnaround time for the pcb
            total_turnaround_time += turnaround_time;                    // Add to the total turnaround time
            total_wait_time += turnaround_time - pcb->total_burst_time;  // Add to the total wait time
            dyn_array_pop_front(arrived_processes);                      // Remove the pcb from the queue
        }
        enqueue_processes(ready_queue, arrived_processes, &current_wait_time, compare_burst); // Add the processes to the arrived_processes queue that have arrived and sort them by burst time
    }
    dyn_array_destroy(arrived_processes); // Free the arrived_processes array
    write_schedule_result(result, total_turnaround_time, total_wait_time, current_wait_time, process_count);

    return true; // Return true because all processes were successfully completed
}