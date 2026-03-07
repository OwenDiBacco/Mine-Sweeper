// Header
#include "stopwatch.h"

static struct timespec getDelta (const struct timespec* currentTime, const struct timespec* startTime)
{
	struct timespec delta =
	{
		.tv_sec = currentTime->tv_sec - startTime->tv_sec,
		.tv_nsec = currentTime->tv_nsec - startTime->tv_nsec
	};

	if (delta.tv_nsec < 0)
	{
		delta.tv_nsec += 1e9;
		delta.tv_sec--;
	}

	return delta;
}

static void update (void* widget)
{
    stopwatch_t* stopwatch = widget;

    clock_gettime (CLOCK_REALTIME, &stopwatch->currentTime);
    struct timespec delta = getDelta (&stopwatch->currentTime, &stopwatch->startTime);

    char* time = malloc (strlen ("00:00:000") + 1);
	if (time == NULL)
		return;

    sprintf (time, "%02lu:%02lu:%03lu",
	    (unsigned long) (delta.tv_sec / 60),
		(unsigned long) (delta.tv_sec % 60),
		(unsigned long) (delta.tv_nsec / 1000000)
	);

    gtk_label_set_text (GTK_LABEL (stopwatch->stopwatch), time);

    free (time);
}

stopwatch_t* stopwatchInit (stopwatchConfig_t* config)
{
    stopwatch_t* stopwatch = malloc (sizeof (stopwatch_t));
    if (stopwatch == NULL)
        return NULL;

    *stopwatch = (stopwatch_t)
    {
        .vmt =
        {
            .update = update,
            .widget = gtk_grid_new ()
        },
        .config = *config,
        .incrementing = true,
        .overlay = gtk_overlay_new (),
        .stopwatch = gtk_label_new ("00:00:000"),
        .startTime = { .tv_sec = 0, .tv_nsec = 0 },
        .currentTime = { .tv_sec = 0, .tv_nsec = 0 }
    };

    clock_gettime (CLOCK_REALTIME, &stopwatch->startTime);

    return stopwatch;
}