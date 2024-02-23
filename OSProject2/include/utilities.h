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

    ProcessControlBlock_t *create_pcb(uint32_t arrival, uint32_t priority, uint32_t remaining_burst_time, bool started, ProcessControlBlock_t *ptr);

    ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    dyn_array_t *create_dyn_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    void print_schedule_result(ScheduleResult_t *result);
    /*End of test helpers*/

    /*Start of analysis helpers*/
    bool str_is_equal(char *str1, char *str2, int char_ct);

    bool is_fcfs(char *str);

    bool is_sjf(char *str);

    bool is_priority(char *str);

    bool is_rr(char *str);

    bool is_srtf(char *str);

    void print_valid_algorithms();
    /*End of analysis helpers*/

    /*Start of process_scheduling helpers*/
    void enqueue_processes(dyn_array_t *ready_queue, dyn_array_t *current_processes, uint32_t *current_wait_time, int (*cmp_fn)(const void *, const void *));

    // Sorts a and b by burst time
    int compare_burst(const void *a, const void *b);

    // Sorts a and b by arrival time. If arrival time is equal then by burst time
    int compare_arrival(const void *a, const void *b);

    void write_schedule_result(ScheduleResult_t *sr, uint32_t total_turnaround_time, uint32_t total_wait_time, uint32_t total_run_time, uint32_t process_count);
    /*End of process_scheduling helpers*/

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */