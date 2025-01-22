#include <benchmarking.h>

static const uint64_t fnv_offset = 14695981039346656037UL;
static const uint64_t fnv_prime = 1099511628211UL;

static const uint16_t bench_data_buckets = 1024;

static size_t next_id = 0;
static struct bench_entry** benchs;
static struct bench_data* data = NULL;
static uint16_t* valid_buckets = NULL;
static uint16_t valid_buckets_count = 0;

/*******************************************************************************
 * Diff functions
 ******************************************************************************/

static double
timespec_diff(struct timespec* end, struct timespec* start)
{
    return (end->tv_sec - start->tv_sec) + (end->tv_nsec - start->tv_nsec) * pow(10, -9);
}

/*******************************************************************************
 * Benchmark one function
 ******************************************************************************/

size_t
bench_init(const char* fn_name)
{
    struct bench_entry* new_be = malloc(sizeof(struct bench_entry));
    new_be->fn_name = fn_name;
    next_id += 1;
    benchs = realloc(benchs, next_id * sizeof(struct bench_entry*));
    benchs[next_id - 1] = new_be;
    return next_id - 1;
}

void
bench_start(size_t id)
{
    benchs[id]->res_start = clock_gettime(CLOCK_MONOTONIC, &benchs[id]->start);
}

void
bench_end(size_t id)
{
    int res_end = clock_gettime(CLOCK_MONOTONIC, &benchs[id]->end);
    assert(benchs[id]->res_start != -1);
    assert(res_end != -1);
    benchs[id]->diff = timespec_diff(&benchs[id]->end, &benchs[id]->start);
    bench_report_one_id(id);
}


/*******************************************************************************
 * Gather data
 ******************************************************************************/

/* FNV-1a hash
 * Sources:
 * * https://benhoyt.com/writings/hash-table-in-c/
 * * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
 */
static uint16_t
fnv_hash(const char* str_to_hash)
{
    uint64_t hash = fnv_offset;
    for (const char* p = str_to_hash; *p; ++p)
    {
        hash ^= (uint64_t) (const char) *p;
        hash *= fnv_prime;
    }
    return hash % bench_data_buckets;
}

/*static void*/
/*insert_entry(struct bench_entry** entries, size_t entries_count, struct bench_entry* new_entry)*/
/*{*/
    /*if (entries_count == 0)*/
    /*{*/
        /*entries[0] = new_entry;*/
        /*return;*/
    /*}*/

    /*size_t bot = 0;*/
    /*size_t top = entries_count - 1;*/
    /*size_t mid;*/
    /*while (bot <= top)*/
    /*{*/
        /*mid = (top - bot) / 2;*/
        /*if (entries[mid]->diff_clock < new_entry->diff_clock)*/
        /*{*/
            /*bot = mid + 1;*/
        /*}*/
        /*else if (new_entry->diff_clock < entries[mid]->diff_clock)*/
        /*{*/
            /*top = mid - 1;*/
        /*}*/
    /*}*/
    /*// Guaranteed to have sufficient space*/
    /*memcpy(entries[mid+1], entries[mid], (entries_count - mid) * sizeof(struct bench_entry*));*/
    /*entries[mid] = new_entry;*/
/*}*/

/*void*/
/*bench_collate_data()*/
/*{*/
    /*if (data == NULL)*/
    /*{*/
        /*data = calloc(bench_data_buckets, sizeof(struct bench_data));*/
    /*}*/
    /*else*/
    /*{*/
        /*memset(data, 0, bench_data_buckets * sizeof(struct bench_data));*/
    /*}*/

    /*uint16_t hash;*/
    /*valid_buckets = malloc(bench_data_buckets * sizeof(uint16_t));*/
    /*for (size_t i = 0; i < next_id; ++i)*/
    /*{*/
        /*hash = fnv_hash(benchs[i]->fn_name);*/
        /*if (data[hash].fn_name == NULL)*/
        /*{*/
            /*data[hash].fn_name = benchs[i]->fn_name;*/
            /*data[hash].call_counts = 0;*/
            /*data[hash].raw_entries = calloc(next_id, sizeof(struct bench_entry*));*/

            /*valid_buckets[valid_buckets_count] = hash;*/
            /*valid_buckets_count += 1;*/
        /*}*/
        /*// TODO handle collisions*/
        /*assert(!strcmp(data[hash].fn_name, benchs[i]->fn_name));*/

        /*insert_entry(data[hash].raw_entries, data[hash].call_counts, benchs[i]);*/
        /*data[hash].call_counts += 1;*/
    /*}*/

    /*for (size_t i = 0; i < valid_buckets_count; ++i)*/
    /*{*/
        /*hash = valid_buckets[i];*/
        /*data[hash].raw_entries = realloc(data[hash].raw_entries, data[hash].call_counts * sizeof(struct bench_entry*));*/
    /*}*/
/*}*/

/*******************************************************************************
 * Printing
 ******************************************************************************/

static void
print_timespec(char* buf, struct timespec* ts)
{
    sprintf(buf, "%lld.%9ld", (long long) ts->tv_sec, ts->tv_nsec);
}

static void
bench_report_one(struct bench_entry* entry)
{

    const unsigned short buf_sz = 30;
    char* buf_st = alloca(buf_sz);
    print_timespec(buf_st, &entry->start);
    char* buf_en = alloca(buf_sz);
    print_timespec(buf_en, &entry->end);
    printf("Func <%s> -- start %s -- end %s -- seconds %f\n",
            entry->fn_name, buf_st, buf_en, entry->diff);
}

void
bench_report_one_id(size_t id)
{
    printf("ID %zu == ", id);
    bench_report_one(benchs[id]);
}

/*void*/
/*bench_report()*/
/*{*/
    /*struct bench_data curr_data;*/
    /*struct bench_entry* curr_entry;*/
    /*double average = 0;*/
    /*double geo = 0;*/
    /*int min;*/
    /*int max;*/
    /*printf("== Benchmarking report ==\n");*/
    /*for (size_t i = 0; i < valid_buckets_count; ++i)*/
    /*{*/
        /*curr_data = data[valid_buckets[i]];*/
        /*for (size_t j = 0; j < curr_data.call_counts; ++j)*/
        /*{*/
            /*curr_entry = curr_data.raw_entries[j];*/
            /*geo += log(curr_entry->diff_clock);*/
            /*average += curr_entry->diff_clock;*/
            /*min = curr_entry->diff_clock < min ? curr_entry->diff_clock : min;*/
            /*max = curr_entry->diff_clock > max ? curr_entry->diff_clock : max;*/
        /*}*/
        /*geo = exp(geo / curr_data.call_counts);*/
        /*average = average / curr_data.call_counts;*/
        /*printf("\t -- %s -- %zu samples\n", curr_data.fn_name, curr_data.call_counts);*/
        /*printf("\t min %d - max %d - geo %f - average %f\n", min, max, geo, average);*/
    /*}*/
/*}*/
