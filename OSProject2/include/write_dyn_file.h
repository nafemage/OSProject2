#ifndef WRITE_FILE_H
#define WRITE_FILE_H

#include <stdint.h>
#include <stdbool.h>

#include "processing_scheduling.h"
#include "dyn_array.h"

#ifdef __cplusplus
extern "C"
{
#endif

    bool write_dyn_array_to_file(dyn_array_t *array, char *filename);

#ifdef __cplusplus
}
#endif

#endif /* WRITE_FILE_H */