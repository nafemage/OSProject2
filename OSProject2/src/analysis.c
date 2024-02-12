#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
#include "processing_scheduling.h"
#include "utilities.h"

#define FCFS "FCFS"
#define P "P"
#define RR "RR"
#define SJF "SJF"

// Add and comment your analysis code in this function.
// THIS IS NOT FINISHED.
int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("%s <pcb file> <schedule algorithm> [quantum]\n", argv[0]);
        printf("Try passing in ../pcb.bin as the file name");
        return EXIT_FAILURE;
    }

    dyn_array_t *a = load_process_control_blocks(argv[1]);
    dyn_array_destroy(a);

    return EXIT_SUCCESS;
}
