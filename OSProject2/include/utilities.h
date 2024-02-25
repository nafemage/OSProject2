#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdbool.h>

#include "processing_scheduling.h"
#include "dyn_array.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /*Start of test helpers*/
    void print_pcb_array(ProcessControlBlock_t *pcb_array, size_t count);

    /**
    *
    * Dynamically allocates memory for a new pcb if 'ptr' is NULL,
    * otherwise, it updates the attributes of the existing pcb pointed to by 'ptr'.
    *
    * @param arrival The arrival time of the process.
    * @param priority The priority of the process.
    * @param remaining_burst_time The remaining burst time of the process.
    * @param started Indicates whether the process has started execution.
    * @param ptr Pointer to an existing pcb. If NULL, memory will be allocated for a new pcb.
    * @return Pointer to the created or updated pcb.
    */
    ProcessControlBlock_t *create_pcb(uint32_t arrival, uint32_t priority, uint32_t remaining_burst_time, bool started, ProcessControlBlock_t *ptr);

    /**
    *
    * Dynamically allocates memory for an array of PCBs and initializes each pcb
    * with the corresponding attributes provided in the arrays. It returns a pointer to the array
    * of PCBs.
    *
    * @param arrivals Pointer to an array containing arrival times of processes.
    * @param priorities Pointer to an array containing priorities of processes.
    * @param remaining_burst_times Pointer to an array containing remaining burst times of processes.
    * @param started Pointer to an array indicating whether processes have started execution.
    * @param count Number of PCBs to create (size of the arrays).
    * @return Pointer to the array of created PCBs. Returns NULL if any of the input arrays is NULL.
    */
    ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    /**
    *
    * Dynamically allocates memory for an array of PCBs using create_pcb_array function,
    * then imports the array into a dynamic array data structure. It returns a pointer to the dynamic array
    * containing the PCBs.
    *
    * @param arrivals Pointer to an array containing arrival times of processes.
    * @param priorities Pointer to an array containing priorities of processes.
    * @param remaining_burst_times Pointer to an array containing remaining burst times of processes.
    * @param started Pointer to an array indicating whether processes have started execution.
    * @param count Number of PCBs to create (size of the arrays).
    * @return Pointer to the dynamic array containing the created PCBs. Returns NULL if any of the input arrays is NULL or if memory allocation fails.
    */
    dyn_array_t *create_dyn_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    /**
    *
    * Prints the schedule result to the desired location. If the location is not provided, prints th stdout.
    *
    * @param result Pointer to schedule result containing results to print.
    * @param file Pointer to FILE where the contents of the result are to be printed to. (If NULL prints to stdout).
    */
    void print_schedule_result(ScheduleResult_t *result, FILE *file);
    /*End of test helpers*/

    /*Start of analysis helpers*/

    /**
    *
    * Compares two strings to check for equality.
    *
    * @param str1 Pointer to the first string.
    * @param str2 Pointer to the second string.
    * @param char_ct The number of characters to compare.
    * @return bool denoting if the two strings are equal.
    */
    bool str_is_equal(char *str1, char *str2, int char_ct);

    /**
    *
    * Checks the given string to see if it matches the first come first serve algorithm.
    *
    * @param str Pointer to the string.
    * @return bool denoting if the string is equal to the first come first serve strings.
    */
    bool is_fcfs(char *str);

    /**
    *
    * Checks the given string to see if it matches the shortest job first algorithm.
    *
    * @param str Pointer to the string.
    * @return bool denoting if the string is equal to the shortest job first strings.
    */
    bool is_sjf(char *str);

    /**
    *
    * Checks the given string to see if it matches the priority algorithm.
    *
    * @param str Pointer to the string.
    * @return bool denoting if the string is equal to the priority strings.
    */
    bool is_priority(char *str);

    /**
    *
    * Checks the given string to see if it matches the round robin algorithm.
    *
    * @param str Pointer to the string.
    * @return bool denoting if the string is equal to the round robin strings.
    */
    bool is_rr(char *str);

    /**
    *
    * Checks the given string to see if it matches the shortest remaining time first algorithm.
    *
    * @param str Pointer to the string.
    * @return bool denoting if the string is equal to the shortest remaining time first strings.
    */
    bool is_srtf(char *str);

    /**
    *
    * Prints the valid strings for each algorithm.
    */
    void print_valid_algorithms();
    /*End of analysis helpers*/

    /*Start of process_scheduling helpers*/

    /**
    *
    * Dequeues processes from the ready queue and enqueues them into the current processes queue
    * based on their arrival time. If no processes are currently in the current processes queue, the function
    * "fast forwards" the current wait time to the arrival time of the first process in the ready queue.
    *
    * @param ready_queue Pointer to the dynamic array representing the ready queue.
    * @param current_processes Pointer to the dynamic array representing the current processes queue.
    * @param current_wait_time Pointer to the variable holding the current wait time.
    * @param cmp_fn Pointer to the comparison function used for sorting processes based on some criteria.
    */
    void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *));

    /**
    *
    * Sorts a and b by burst time.
    * 
    * @param a Pointer to the first pcb to compare.
    * @param b Pointer to the second pcb to compare.
    * @return int denoting inequality comparision.
    */
    int compare_burst(const void *a, const void *b);

    /**
    *
    * Sorts a and b by arrival time.
    * 
    * @param a Pointer to the first pcb to compare.
    * @param b Pointer to the second pcb to compare.
    * @return int denoting inequality comparision.
    */
    int compare_arrival(const void *a, const void *b);

    /**
    *
    * Sorts a and b by arrival time (if equal then by burst time).
    * 
    * @param a Pointer to the first pcb to compare.
    * @param b Pointer to the second pcb to compare.
    * @return int denoting inequality comparision.
    */
    int compare_arrival_burst(const void *a, const void *b);

    /**
    * 
    * Updates the fields of the schedule result.
    * 
    * @param sr Pointer to the schedule result to be updated.
    * @param total_turnaround_time Total turnaround time of the algorithm.
    * @param total_wait_time Total wait time of the algorithm.
    * @param total_run_time Total run time of the algorithm.
    * @param process_count Number of processes in the schedule.
    */
    void write_schedule_result(ScheduleResult_t *sr, uint32_t total_turnaround_time, uint32_t total_wait_time, uint32_t total_run_time, uint32_t process_count);

    /**
    * 
    * Returns a FILE to the readme file (NULL if error).
    * 
    * @return Pointer to a FILE of the readme (NULL if error).
    */
    FILE *get_readme();

    /**
    * 
    * Seeks to the line number in the file.
    * 
    * @param file Pointer to the file to seek.
    * @param line_number Number of lines to seek.
    */
    void seek_file(FILE *file, int line_number);

    /**
    * 
    * Prints the result to the readme.
    * 
    * @param result Pointer to the schedule result to print.
    * @param line_number The line number to print the result on.
    */
    bool print_to_readme(ScheduleResult_t *result, int line_number);
    /*End of process_scheduling helpers*/

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */