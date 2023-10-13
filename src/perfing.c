#include "perfing.h"

struct time_log* perf_log = NULL;
size_t perf_log_count = 0;

void
record_time(char* t_point)
{
    perf_log_count += 1;
    perf_log = realloc(perf_log, perf_log_count * sizeof(struct time_log));
    struct timespec new_ts;
    clock_gettime(CLOCK_REALTIME, &new_ts);
    struct time_log new_log = {t_point, new_ts.tv_nsec};
    perf_log[perf_log_count - 1] = new_log;
}

long
compute_time(char* t_point_1, char* t_point_2)
{
    long t_val_1;
    long t_val_2;
    bool found_1 = false;
    bool found_2 = false;
    for (size_t i = 0; i < perf_log_count; ++i)
    {
        if (!strcmp(perf_log[i].t_point, t_point_1))
        {
            assert(!found_1);
            found_1 = true;
            t_val_1 = perf_log[i].t_val;
        }
        else if(!strcmp(perf_log[i].t_point, t_point_2))
        {
            assert(!found_2);
            found_2 = true;
            t_val_2 = perf_log[i].t_val;
        }
        if (found_1 && found_2)
        {
            break;
        }
    }
    assert(found_1);
    assert(found_2);
    return t_val_2 - t_val_1;
}

void
print_interval(char* t_point_1, char* t_point_2)
{
    const unsigned short interval_groups = 3;
    long interval_time = compute_time(t_point_1, t_point_2);
    assert(interval_time >= 0);
    char* format_interval_time = malloc(log10(interval_time) + log10(interval_time) / interval_groups + 1);
    unsigned short index = 0;
    while(interval_time > 0)
    {

        

    }
    printf("INTERVAL TIME [`%s` to `%s`] : %ld ns\n", t_point_1, t_point_2, compute_time(t_point_1, t_point_2));
}


