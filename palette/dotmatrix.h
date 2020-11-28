#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void init_dotmatrix();
void dotmatrix_write(int count);
void close_dotmatrix();
