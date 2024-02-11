#ifndef PCB_H
#define PCB_H

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

#ifdef __cplusplus
}
#endif

#endif /* PCB_H */