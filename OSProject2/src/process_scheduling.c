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
    
    // Iterate through the processes
    for(size_t i = 0; i < num_processes; ++i){
        ProcessControlBlock_t *pcb = dyn_array_at(ready_queue, i);

        // Mark PCB as started
        pcb->started = true;

        // If pcb hasn't "arrived" yet, fast forward to its arrival
        if(total_run_time < pcb->arrival) 
        {
            total_run_time = pcb->arrival;
        }

        // Update statistics
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
    write_schedule_result(result, total_turnaround_time, total_waiting_time, total_run_time, num_processes);

    return true;
}

bool shortest_job_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    // Error checking
    if (ready_queue == NULL || result == NULL || dyn_array_size(ready_queue) == 0)
        return false;

    // Sort the ready queue based on remaining burst time (shortest job first)
    dyn_array_sort(ready_queue, compare_arrival_burst);

    // Initialize variables for tracking statistics
    float total_waiting_time = 0.0;
    float total_turnaround_time = 0.0;
    unsigned long total_run_time = 0;
    int starting_queue_size = dyn_array_size(ready_queue);

    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(ready_queue) > 0)
    {
        // Find the shortest arrival time among all PCBs
        const ProcessControlBlock_t *first_pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, 0); // *Won't return NULL because loop guard states there is at leats 1 element in the queue
        uint32_t shortest_burst_time = first_pcb->remaining_burst_time;
        uint32_t shortest_arrival_time = first_pcb->arrival;

        // Move total_run_time forward by the shortest arrival time
        total_run_time = (shortest_arrival_time > total_run_time) ? shortest_arrival_time : total_run_time;

        // Find the shortest remaining process by looking at arrival time and burst time
        size_t pcb_index = 0;
        for (size_t i = 0; i < dyn_array_size(ready_queue); ++i)
        {
            const ProcessControlBlock_t *pcb = (const ProcessControlBlock_t *)dyn_array_at(ready_queue, i); // *Won't return NULL because loop stays within bounds of array

            // If the pcb has arrived
            if (pcb->arrival <= total_run_time)
            {
                // If the pcb has less burst time than the shortest burst time, update the shortest found burst time and the index
                if(pcb->remaining_burst_time < shortest_burst_time)
                {
                    shortest_burst_time = pcb->remaining_burst_time;
                    pcb_index = i;
                }
            } else {
                break; // The current pcb and those after it have not arrived and don't need to be considered
            }
        }

        // Get the PCB with the shortest remaining burst time
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, pcb_index); // *Won't return NULL because pcb_index is always set to a valid index within the previous loop

        // Mark PCB as started
        pcb->started = true;

        // Update statistics
        total_run_time += pcb->remaining_burst_time;
        float turnaround_time = total_run_time - pcb->arrival;
        total_turnaround_time += turnaround_time;
        total_waiting_time += turnaround_time - pcb->remaining_burst_time;

        // Execute the process
        virtual_cpu(pcb, pcb->remaining_burst_time);

        // Mark PCB as completed
        pcb->completed = true;

        // Remove the processed PCB from the ready_queue
        dyn_array_erase(ready_queue, pcb_index);
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

    // Sort queue based on arrival time
    dyn_array_sort(ready_queue, compare_arrival);

    // Initialize array for the arrived processes
    dyn_array_t *arrived_processes = dyn_array_create(0, sizeof(ProcessControlBlock_t), NULL); 
    if(arrived_processes == NULL)
    {
        return false;
    }

    // Initialize the arrived processes array to hold the first processes
    ProcessControlBlock_t *first_pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0); // *Won't return NULL because the ready_queue size was already checked and handled if 0
    bool success = dyn_array_push_back(arrived_processes, first_pcb);

    // If an error occured, delete the arrived_processes array and return false
    if(!success)
    {
        dyn_array_destroy(arrived_processes);
        return false;
    }
    total_run_time = first_pcb->arrival;
    success = dyn_array_erase(ready_queue, 0);

    // If an error occured, delete the arrived_processes array and return false
    if(!success)
    {
        dyn_array_destroy(arrived_processes);
        return false;
    }
    
    // Process each PCB in the ready queue until all are removed
    while (dyn_array_size(arrived_processes) > 0) 
    {
        // Get the next pcb in line
        ProcessControlBlock_t *pcb = (ProcessControlBlock_t *)dyn_array_at(arrived_processes, 0); // *Won't return NULL because of loop guard stating there is at least 1 element in the queue

        // Mark PCB as started
        if(!pcb->started){
            pcb->started = true;
        }
        
        // Depending on whether the process will be executed in its entirety...
        if (pcb->remaining_burst_time <= quantum)
        {
           
            // Update statistics
            total_run_time += pcb->remaining_burst_time;
            float turnaround_time = total_run_time - pcb->arrival;
            total_turnaround_time += turnaround_time;
            total_waiting_time += turnaround_time - pcb->total_burst_time;

            // Execute the process
            virtual_cpu(pcb, pcb->remaining_burst_time);

            // Mark PCB as completed
            pcb->completed = true;

            // Remove the processed PCB from the ready_queue
            dyn_array_erase(arrived_processes, 0);
        }
        else
        {
            // Update statistics
            total_run_time += quantum;

            // Execute the process for the quantum amount
            virtual_cpu(pcb, quantum);

            // While there are still processes in the ready queue
            while(dyn_array_size(ready_queue) > 0)
            {
                // Get the first process
                ProcessControlBlock_t *front_pcb = (ProcessControlBlock_t *)dyn_array_at(ready_queue, 0); // *Won't return NULL because of loop guard stating there is at least 1 element in the queue

                // If there are no processes in the arrived queue and the front process hasn't "arrived" yet, fast forward to the arrival time
                if(dyn_array_size(arrived_processes) == 0 && total_run_time < front_pcb->arrival){
                    total_run_time = front_pcb->arrival;
                }

                // If the pcb has arrived, add it to the queue and remove it from the ready queue
                if(front_pcb->arrival <= total_run_time)
                {
                    success = dyn_array_push_back(arrived_processes, front_pcb);
                    // If an error occured, delete the arrived_processes array and return false
                    if(!success)
                    {
                        dyn_array_destroy(arrived_processes);
                        return false;
                    }
                    success = dyn_array_erase(ready_queue, 0);
                    // If an error occured, delete the arrived_processes array and return false
                    if(!success)
                    {
                        dyn_array_destroy(arrived_processes);
                        return false;
                    }
                } else {
                    // No pcbs are ready yet
                    break;
                }
            }

            // Move PCB to the end of the queue, but DONT erase it
            success = dyn_array_push_back(arrived_processes, pcb);
            // If an error occured, delete the arrived_processes array and return false
            if(!success)
            {
                dyn_array_destroy(arrived_processes);
                return false;
            }
            success = dyn_array_erase(arrived_processes, 0);
            // If an error occured, delete the arrived_processes array and return false
            if(!success)
            {
                dyn_array_destroy(arrived_processes);
                return false;
            }
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
        uint32_t remaining_burst_time;                                                //Stores the remaining burst time
        uint32_t priority;                                                            //Stores the priority
        uint32_t arrival;                                                             //Store the arrival time
        elements_read = fread(&remaining_burst_time, sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the burst time) and update the pcb's burst time
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
        elements_read = fread(&priority, sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the priority) and update the pcb's priority
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
        elements_read = fread(&arrival, sizeof(uint32_t), 1, fp); // Read the next line in the file (which corresponds to the arrival) and update the pcb's arrival
        if (elements_read != 1)
        {
            free(pcb_array); // Free the pcb_array since an invalid number of elements were read (most likely 0 or an error) meaning the file is invalid
            return NULL;     // Return NULL if less than 1 or more than 1 elements were read
        }
        create_pcb( arrival,  priority,  remaining_burst_time, false, pcb); //Initialize the pcb with the read values
    }
    fclose(fp);                                                                                           // Close the file
    dyn_array_t *dyn_array = dyn_array_import(pcb_array, pcb_count, sizeof(ProcessControlBlock_t), NULL); // Create a dyn_array out of the pcb_array
    free(pcb_array);                                                                                      // Free the pcb_array
    return dyn_array;                                                                                     // Return the dyn_array
}

bool shortest_remaining_time_first(dyn_array_t *ready_queue, ScheduleResult_t *result)
{
    if (ready_queue == NULL || dyn_array_size(ready_queue) == 0 || result == NULL)
    {
        return false; // Return false if ready_queue is NULL or size is 0 (no pcbs to process) or result is NULL (no memory allocated for the result)
    }
    uint32_t process_count = ready_queue->size; // The number of processes in the queue
    uint32_t total_turnaround_time = 0;         // The sum of all turnaround times
    uint32_t total_wait_time = 0;               // The sum of all wait times

    dyn_array_sort(ready_queue, compare_arrival_burst); // sort array by arrival time (if equal then by burst time)

    dyn_array_t *arrived_processes = dyn_array_create(ready_queue->capacity, sizeof(ProcessControlBlock_t), NULL); // dyn_array fors holding the processes that have arrived
    if(arrived_processes == NULL)
    {
        return false; //Return false if arrived_processes array could not be allocated
    }
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
    write_schedule_result(result, total_turnaround_time, total_wait_time, current_wait_time, process_count); //Write the results to the result struct

    return true; // Return true because all processes were successfully completed
}