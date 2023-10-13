#ifndef _PERFING_H
#define _PERFING_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

struct time_log
{
    char* t_point;
    long t_val;
};

void record_time(char*);
long compute_time(char*, char*);
void print_interval(char*, char*);

#endif // _PERFING_H
