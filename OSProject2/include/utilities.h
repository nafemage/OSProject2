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

    void print_pcb_array(ProcessControlBlock_t *pcb_array, size_t count);

    ProcessControlBlock_t *create_pcb(uint32_t arrival, uint32_t priority, uint32_t remaining_burst_time, bool started, ProcessControlBlock_t *ptr);

    ProcessControlBlock_t *create_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    dyn_array_t *create_dyn_pcb_array(uint32_t *arrivals, uint32_t *priorities, uint32_t *remaining_burst_times, bool *started, int count);

    void print_schedule_result(ScheduleResult_t *result);

    /*String checks for algorithm passed in through command line when running analysis exe*/
    bool str_is_equal(char *str1, char *str2, int char_ct);

    bool is_fcfs(char *str);

    bool is_sjf(char *str);

    bool is_priority(char *str);

    bool is_rr(char *str);

    bool is_srtf(char *str);

    void print_valid_algorithms();

#ifdef __cplusplus
}
#endif

#endif /* UTIL_H */