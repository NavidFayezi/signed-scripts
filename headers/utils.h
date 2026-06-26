#ifndef UTILS_H
#define UTILS_H

#include "common.h"


int binary_to_base64(unsigned char *input, 
                     int input_len, 
                     unsigned char *output);

off_t get_file_size(int fd);

int read_file(const char * filename, char **file_data);

#endif // UTILS_H