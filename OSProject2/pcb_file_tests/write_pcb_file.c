#include <stdio.h>
#include <stdlib.h>

#include "dyn_array.h"

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