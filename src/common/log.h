
#ifndef LOG_H
#define LOG_H

typedef void (*est_pfn_t)(char, void *);    /* Output function */

enum
{
    EST_LL_NONE,
    EST_LL_ERROR,
    EST_LL_INFO,
    EST_LL_DEBUG,
    EST_LL_VERBOSE
};

void est_log(const char *fmt, ...);
bool est_log_prefix(int ll, const char *file, int line, const char *fname);
void est_log_set(int log_level);
void est_hexdump(const void *buf, size_t len);
void est_log_set_fn(est_pfn_t fn, void *param);
void est_log_set(int log_level);

#define EST_LOG(level, args)                                       \
    do                                                             \
    {                                                              \
        if (est_log_prefix((level), __FILE__, __LINE__, __func__)) \
            est_log args;                                          \
    } while (0)


#define LOGE(args) est_log(EST_LL_ERROR, args)
#define LOGI(args) est_log(EST_LL_INFO, args)
#define LOGD(args) est_log(EST_LL_DEBUG, args)
#define LOGV(args) est_log(EST_LL_VERBOSE, args)

#endif