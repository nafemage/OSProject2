#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"

/*Start of test helpers*/
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

// Prints to stdout if file is NULL
void print_schedule_result(ScheduleResult_t *result, FILE *file)
{
    FILE *output = file == NULL ? stdout : file;
    fprintf(output, "Average Waiting Time: %f\n", result->average_waiting_time);
    fprintf(output, "Average Turnaround Time: %f\n", result->average_turnaround_time);
    fprintf(output, "Total Run time: %lu\n", result->total_run_time);
}
/*End of test helpers*/

/*Start of analysis helpers*/

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"
#define SRTF "SRTF"

bool str_is_equal(char *str1, char *str2, int char_ct)
{
    return strncmp(str1, str2, char_ct) == 0;
}

bool is_fcfs(char *str)
{
    return str_is_equal(str, FCFS, 5) || str_is_equal(str, "first_come_first_serve", 23);
}

bool is_sjf(char *str)
{
    return str_is_equal(str, SJF, 4) || str_is_equal(str, "shortest_job_first", 19);
}

bool is_priority(char *str)
{
    return str_is_equal(str, P, 2) || str_is_equal(str, "priority", 10);
}

bool is_rr(char *str)
{
    return str_is_equal(str, RR, 3) || str_is_equal(str, "round_robin", 12);
}

bool is_srtf(char *str)
{
    return str_is_equal(str, SRTF, 5) || str_is_equal(str, "shortest_remaining_time_first", 30);
}

void print_valid_algorithms()
{
    printf("The valid algorthims are:\n");
    printf("First come first serve: \'%s\' OR \'first_come_first_serve\'.\n", FCFS);
    printf("Shortest job first: \'%s\' OR \'shortest_job_first\'.\n", SJF);
    printf("Round robin: \'%s\' OR \'round_robin\'.\n", RR);
    printf("Shortest remaining time first: \'%s\' OR \'shortest_remaining_time_first\'.\n", SRTF);
}
/*End of analysis helpers*/

/*Start of process_scheduling helpers*/
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

int compare_burst(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a; // Cast the "a" variable to a pcb
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b; // Cast the "b" variable to a pcb
    return pcb_a->remaining_burst_time - pcb_b->remaining_burst_time;      // The pcb with the shortest burst time should be processed before the other
}

int compare_arrival_burst(const void *a, const void *b)
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

int compare_arrival(const void *a, const void *b)
{
    const ProcessControlBlock_t *pcb_a = (const ProcessControlBlock_t *)a; // Cast the "a" variable to a pcb
    const ProcessControlBlock_t *pcb_b = (const ProcessControlBlock_t *)b; // Cast the "b" variable to a pcb
    return pcb_a->arrival - pcb_b->arrival;
}

void write_schedule_result(ScheduleResult_t *sr, uint32_t total_turnaround_time, uint32_t total_wait_time, uint32_t total_run_time, uint32_t process_count)
{
    sr->average_turnaround_time = (float)total_turnaround_time / process_count; // Calculate and store the average turnaround time
    sr->average_waiting_time = (float)total_wait_time / process_count;          // Calculate and store the average wait time
    sr->total_run_time = total_run_time;
}

#define READMELOC "../readme.md"

FILE *get_readme()
{
    // Open the readme.md file for both reading and writing
    FILE *readme_file = fopen(READMELOC, "r+");
    if (readme_file == NULL)
    {
        fprintf(stderr, "Error opening ../readme.md for reading and writing\n");
        return NULL;
    }
    return readme_file;
}

void seek_file(FILE *file, int line_number)
{
    // Seek to the beginning of line 'line_number'
    fseek(file, 0, SEEK_SET);
    for (int i = 0; i < line_number; ++i)
        while (fgetc(file) != '\n')
            ;
}

bool print_to_readme(ScheduleResult_t *result, int line_number)
{
    FILE *readme_file = get_readme();
    if (readme_file == NULL)
    {
        return false;
    }
    seek_file(readme_file, line_number - 1);
    print_schedule_result(result, readme_file);
    // Close the file
    fclose(readme_file);
    return true;
}
/*End of process_scheduling helpers*/