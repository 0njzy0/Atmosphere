#ifndef FUSEE_SD_UTILS_H
#define FUSEE_SD_UTILS_H

#include "utils.h"
#include "sdmmc.h"

size_t read_sd_file(void *dst, size_t dst_size, const char *filename);

#endif
