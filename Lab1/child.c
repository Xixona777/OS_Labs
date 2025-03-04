#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "func.h"

int child_process(void) {
    int buf_size = 2;
    char *input_buffer = malloc(buf_size);
    memset(input_buffer, 0, buf_size);
    char output_buffer[64];

    if (!input_buffer) {
        return -1;
    }
    while (1) {
        int count = read_line(STDIN_FILENO, &input_buffer, &buf_size);
        if (count <= 0)
            break;
        float res = 0.0f;
        char *ptr = input_buffer;
        while (*ptr) {
            float f = strtof(ptr, NULL);
            res += f;
            ptr = strchr(ptr, ' ');
            if (!ptr) 
                break;
            ptr++;
        }
        int n = snprintf(output_buffer, sizeof(output_buffer), "%.2f\n", res);
        write(STDOUT_FILENO, output_buffer, n);
    }
    free(input_buffer);
    return 0;
}
