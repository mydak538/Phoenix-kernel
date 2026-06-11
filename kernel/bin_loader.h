#ifndef BIN_LOADER_H
#define BIN_LOADER_H

#include <stdint.h>

int bin_load(uint8_t* bin_data, uint32_t size);
int bin_execute(uint8_t* bin_data, uint32_t size);

#endif
