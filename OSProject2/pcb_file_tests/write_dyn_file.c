#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"
// #include "processing_scheduling.h"
// #include "utilities.h"

// bool write_pcb_values_to_file(dyn_array_t *array, char *filename)
// {
//     if (array == NULL || filename == NULL)
//     {
//         return false;
//     }
//     FILE *file = fopen(filename, "wb");
//     if (!file)
//     {
//         return false;
//     }
//     size_t elements_written;
//     elements_written = fwrite(&(array->size), sizeof(uint32_t), 1, file);
//     if (elements_written != 1)
//     {
//         return false;
//     }
//     for (size_t i = 0; i < array->size; i++)
//     {
//         ProcessControlBlock_t pcb = ((ProcessControlBlock_t *)(array->array))[i];
//         elements_written = fwrite(&pcb.remaining_burst_time, sizeof(uint32_t), 1, file);
//         if (elements_written != 1)
//         {
//             return false;
//         }
//         elements_written = fwrite(&pcb.priority, sizeof(uint32_t), 1, file);
//         if (elements_written != 1)
//         {
//             return false;
//         }
//         elements_written = fwrite(&pcb.arrival, sizeof(uint32_t), 1, file);
//         if (elements_written != 1)
//         {
//             return false;
//         }
//     }
//     fclose(file);
//     return true;
// }

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("%s <file_destination> <count> <pcb_1_burst_time> <pcb_1_priority> <pcb_1_arrival> <...>\n", argv[0]);
        return EXIT_FAILURE;
    }
    char path[50] = "../pcb_file_tests/files/";
    char *file_path = strcat(path, argv[1]);

    FILE *file = fopen(file_path, "w");
    if (!file)
    {
        printf("Open fail: %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    size_t elements_written;
    for (int i = 2; i < argc; i++)
    {
        uint32_t value = (uint32_t)atoi(argv[i]);
        printf("Value: %u\n", value);
        elements_written = fwrite(&value, sizeof(uint32_t), 1, file);
        if (elements_written != 1)
        {
            fclose(file);
            printf("Loop fail\n");
            return EXIT_FAILURE;
        }
    }
    fclose(file);
    printf("Success\n");

    return EXIT_SUCCESS;
}