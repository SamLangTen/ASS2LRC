#include <stdlib.h>
#include <ass/ass.h>
#include <ass/ass_types.h>

int ass_event_asccmp(const ASS_Event *a, const ASS_Event *b)
{
    return (a->Start > b->Start) ? 1 : 0;
}

int main(int argc, char *argv[])
{
    ASS_Library *ass_library = ass_library_init();
    if (!ass_library)
    {
        printf("Failed to load ass_library\n");
        exit(1);
    }

    ASS_Track *track = ass_read_file(ass_library, argv[1], NULL);
    if (!track)
    {
        printf("Failed to load ASS file\n");
        exit(1);
    }

    qsort(track->events, track->n_events, sizeof(ASS_Event), ass_event_asccmp);
    for (int i = 0; i < track->n_events; i++)
    {
        printf("%d %lld %s\n", i + 1, track->events[i].Start, track->events[i].Text);
    }

    ass_free_track(track);
    ass_library_done(ass_library);
    return 0;
}