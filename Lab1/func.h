#ifndef __FUNC_H__
#define __FUNC_H__

#include <stddef.h>

int child_process(void);
char *strnchr(const char *buf, char c, size_t len);
int read_line(int fd, char **buf, int *buf_size);
int print(int fd, const char *s);

#endif
