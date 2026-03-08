#ifndef STOPWATCH_H
#define STOPWATCH_H

// Includes
#include <stddef.h>
#include <stdbool.h>
#include <gtk/gtk.h>


typedef void (stopwatchUpdate_t) (void* widget);
typedef struct
{
    stopwatchUpdate_t* update;
    GtkWidget* widget;

} stopwatchVmt_t;

typedef struct
{
    size_t width;
    size_t height;

} stopwatchConfig_t;

typedef struct
{

    stopwatchVmt_t vmt;
    stopwatchConfig_t config;

    GtkWidget* overlay;
    GtkWidget* stopwatch;

    bool incrementing;

    struct timespec startTime;
    struct timespec currentTime;

} stopwatch_t;

char* formatTime (struct timespec time);

struct timespec stopwatchGetDelta (const struct timespec* currentTime, const struct timespec* startTime);

stopwatch_t* stopwatchInit (stopwatchConfig_t* config);

#endif // STOPWATCH_H